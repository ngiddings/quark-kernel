#include "kernel.h"
#include "mmgr.h"
#include "heap.h"
#include "stdio.h"
#include "elf.h"
#include "syscalls.h"
#include "string.h"
#include "config.h"
#include "system.h"
#include "platform/interrupts.h"
#include "platform/context.h"
#include "platform/putc.h"
#include "types/status.h"

struct kernel_t kernel;

void kernel_initialize(struct boot_info_t *boot_info)
{
    insert_region(&boot_info->map, (physaddr_t)&_kernel_pstart, (physaddr_t)&_kernel_pend - (physaddr_t)&_kernel_pstart, M_UNAVAILABLE);
    initialize_page_stack(&boot_info->map, (physaddr_t*)&_kernel_end);
    kminit(page_stack_top(), 0xFFC00000 - (size_t)page_stack_top(), page_size);
    initialize_screen();
    printf("***%s***\n", PACKAGE_STRING);
    printf("Type\t\tLocation\t\tSize\n");
    for (size_t i = 0; i < boot_info->map.size && boot_info->map.array[i].size > 0; i++)
    {
        printf("%i\t\t\t%08x\t\t%u\n", boot_info->map.array[i].type, boot_info->map.array[i].location, boot_info->map.array[i].size);
    }

    kernel.active_process = NULL;
    kernel.next_pid = 1;
    kernel.process_table = NULL;
    if(construct_priority_queue(&kernel.priority_queue, 512) != S_OK)
    {
        panic("Failed to construct priority queue.");
    }
    memset(kernel.syscall_table, 0, sizeof(struct syscall_t) * MAX_SYSCALL_ID);
    set_syscall(SYSCALL_TEST, 1, 0, test_syscall);
    set_syscall(SYSCALL_MMAP, 3, 0, mmap);
    set_syscall(SYSCALL_MUNMAP, 2, 0, munmap);
    for(int i = 0; i < boot_info->module_count; i++)
    {
        if(load_module(&boot_info->modules[i]) != S_OK)
        {
            panic("Failed to load modules.");
        }
    }

    if(initialize_interrupts() != S_OK)
    {
        panic("Failed to initialize interrupts.");
    }

    /*asm("mov $281, %ax;"
        "mov %ax, %ds");

    asm("hlt");*/

    irq_enable();
    load_context(next_process(NULL));
}

int set_syscall(int id, int arg_count, int pid, void *func_ptr)
{
    if(id < 0 || id > MAX_SYSCALL_ID)
    {
        return S_OUT_OF_BOUNDS;
    }
    else if(kernel.syscall_table[id].defined)
    {
        return S_INVALID_ARGUMENT;
    }
    else if(arg_count < 0 || arg_count > 3)
    {
        return S_INVALID_ARGUMENT;
    }
    else if(pid != 0 && avl_get(kernel.process_table, pid) == NULL)
    {
        return S_DOESNT_EXIST;
    }
    else if(func_ptr == NULL)
    {
        return S_NULL_POINTER;
    }
    kernel.syscall_table[id].defined = true;
    kernel.syscall_table[id].arg_count = arg_count;
    kernel.syscall_table[id].process_id = pid;
    kernel.syscall_table[id].func_ptr_0 = func_ptr;
    return S_OK;
}

size_t do_syscall(enum syscall_id_t id, syscall_arg_t arg1, syscall_arg_t arg2, syscall_arg_t arg3)
{
    if(id < 0 || id > MAX_SYSCALL_ID)
    {
        return S_BAD_SYSCALL;
    }
    else if(!kernel.syscall_table[id].defined)
    {
        return S_BAD_SYSCALL;
    }
    bool switched_address_space = false;
    if(kernel.syscall_table[id].process_id > 0)
    {
        struct process_t *callee = avl_get(kernel.process_table, kernel.syscall_table[id].process_id);
        if(callee == NULL)
        {
            kernel.syscall_table[id].defined = false;
            return S_BAD_SYSCALL;
        }
        paging_load_address_space(callee->page_table);
        switched_address_space = true;
    }
    size_t result;
    switch(kernel.syscall_table[id].arg_count)
    {
    case 0:
        result = kernel.syscall_table[id].func_ptr_0();
        break;
    case 1:
        result = kernel.syscall_table[id].func_ptr_1(arg1);
        break;
    case 2:
        result = kernel.syscall_table[id].func_ptr_2(arg1, arg2);
        break;
    case 3:
        result = kernel.syscall_table[id].func_ptr_3(arg1, arg2, arg3);
        break;
    }
    if(switched_address_space)
    {
        paging_load_address_space(kernel.active_process->page_table);
    }
    return result;
}

int load_module(struct module_t *module)
{
    physaddr_t module_address_space = create_address_space();
    if(module_address_space == S_OUT_OF_MEMORY) {
        panic("failed to create address space for module: out of memory");
    }
    paging_load_address_space(module_address_space);
    void *const load_base = (void*)0x80000000;
    size_t load_offset = 0;
    for(physaddr_t p = module->start & ~(page_size - 1); p < module->end; p += page_size)
    {
        int status = map_page(load_base + load_offset, p, PAGE_RW);
        switch(status)
        {
        case S_OUT_OF_MEMORY:
            panic("ran out of memory while mapping module");
        case S_OUT_OF_BOUNDS:
            panic("got out-of-bounds error while mapping module");
        }
        load_offset += page_size;

    }
    int status = load_program(load_base);
    switch(status)
    {
    case S_OUT_OF_MEMORY:
        panic("ran out of memory while reading ELF file");
    case S_OUT_OF_BOUNDS:
        panic("got out-of-bounds error while reading ELF file");
    }
    void *module_entry = ((struct elf_file_header_t*)load_base)->entry;
    printf("loaded module with entry point %08x\n", (unsigned int)module_entry);
    load_offset = 0;
    for(physaddr_t p = module->start & ~(page_size - 1); p < module->end; p += page_size)
    {
        int status = unmap_page(load_base + load_offset);
        switch(status)
        {
        case S_OUT_OF_MEMORY:
            panic("ran out of memory while unmapping module");
        case S_OUT_OF_BOUNDS:
            panic("got out-of-bounds error while unmapping module");
        }
        load_offset += page_size;
    }
    if(add_process(module_entry, 1, current_address_space()) > 0)
    {
        return S_OK;
    }
    else
    {
        return -1;
    }
}

int active_process()
{
    if(kernel.active_process == NULL)
    {
        return 0;
    }
    else
    {
        return kernel.active_process->resource_id;
    }
}

int add_process(void *program_entry, int priority, physaddr_t address_space)
{
    struct process_t *new_process = (struct process_t*) kmalloc(sizeof(struct process_t));
    if(new_process == NULL)
    {
        return 0;
    }
    struct process_context_t *initial_context = initialize_context(program_entry);
    new_process->priority = priority;
    new_process->resource_id = kernel.next_pid;
    new_process->page_table = address_space;
    new_process->state = initial_context;
    kernel.process_table = avl_insert(kernel.process_table, new_process->resource_id, new_process);
    queue_insert(&kernel.priority_queue, new_process, new_process->priority);
    kernel.next_pid++;
    return new_process->resource_id;
}

struct process_context_t *next_process(struct process_context_t *prev_state)
{  
    if(prev_state != NULL)
    {
        kernel.active_process->state = prev_state;
        queue_insert(&kernel.priority_queue, kernel.active_process, kernel.active_process->priority);
    }
    kernel.active_process = extract_min(&kernel.priority_queue);
    if(kernel.active_process != NULL)
    {
        paging_load_address_space(kernel.active_process->page_table);
        printf("entering process %08x cr3=%08x state=%08x.\n", kernel.active_process, kernel.active_process->page_table, kernel.active_process->state);
        return kernel.active_process->state;
    }
    panic("no processes available to enter!");
}

int terminate_process(size_t process_id)
{
    struct process_t *process = avl_get(kernel.process_table, process_id);
    if(process == NULL)
    {
        return S_DOESNT_EXIST;
    }
    if(kernel.active_process == process)
    {
        kernel.active_process = NULL;
    }
    kernel.process_table = avl_remove(kernel.process_table, process_id);
    queue_remove(&kernel.priority_queue, process);
    destroy_context(process->state);
    kfree(process);
    return S_OK;
}

/*
int accept_message(size_t process_id, struct message_t *message)
{
    if(message == NULL)
    {
        return S_NULL_POINTER;
    }
    else if(kernel->resource_table->limit >= process_id)
    {
        return S_OUT_OF_BOUNDS;
    }
    else if(kernel->resource_table->array[process_id].type != RESOURCE_PROCESS)
    {
        return S_INVALID_ARGUMENT;
    }
    struct process_t *process = &kernel->resource_table->array[process_id].process;
    process->state = PROCESS_WAITING;
    process->message = message;
    if(kernel->active_process == process)
    {
        kernel->active_process = NULL;
    }
    queue_remove(kernel->priority_queue, process);
    return S_OK;
}

int send_message(size_t process_id, const struct message_t *message)
{
    if(kernel == NULL || message == NULL)
    {
        return S_NULL_POINTER;
    }
    else if(kernel->resource_table->limit >= process_id)
    {
        return S_OUT_OF_BOUNDS;
    }
    else if(kernel->resource_table->array[process_id].type != RESOURCE_PROCESS)
    {
        return S_INVALID_ARGUMENT;
    }
    struct process_t *process = &kernel->resource_table->array[process_id].process;
    if(process->state != PROCESS_WAITING || process->message == NULL)
    {
        return S_BUSY;
    }
    queue_insert(kernel->priority_queue, kernel->active_process);
    struct message_t buffer = *message;

}
*/

void panic(const char *message)
{
    printf("panic: %s", message);
    asm("cli");
    while(1) asm("hlt");
}

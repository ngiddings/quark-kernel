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
    kminit(&_kernel_start, page_stack_top(), 0xFFC00000 - (size_t)&_kernel_start, 64);
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
    kernel.port_table = NULL;
    if(construct_priority_queue(&kernel.priority_queue, 512) != S_OK)
    {
        panic("Failed to construct priority queue.");
    }
    memset(kernel.syscall_table, 0, sizeof(struct syscall_t) * MAX_SYSCALL_ID);
    set_syscall(SYSCALL_TEST, 1, 0, test_syscall);
    set_syscall(SYSCALL_MMAP, 3, 0, mmap);
    set_syscall(SYSCALL_MUNMAP, 2, 0, munmap);
    set_syscall(SYSCALL_SEND, 3, 0, send);
    set_syscall(SYSCALL_RECEIVE, 2, 0, receive);
    set_syscall(SYSCALL_OPEN_PORT, 1, 0, openport);
    set_syscall(SYSCALL_CLOSE_PORT, 1, 0, closeport);
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

    irq_enable();
    load_context(next_process());
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

size_t do_syscall(enum syscall_id_t id, syscall_arg_t arg1, syscall_arg_t arg2, syscall_arg_t arg3, void *pc, void *stack, unsigned long flags)
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
    set_context_pc(kernel.active_process->ctx, pc);
    set_context_stack(kernel.active_process->ctx, stack);
    set_context_flags(kernel.active_process->ctx, flags);
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
        return kernel.active_process->pid;
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
    new_process->pid = kernel.next_pid;
    new_process->page_table = address_space;
    new_process->state = PROCESS_ACTIVE;
    new_process->message_buffer = NULL;
    new_process->ctx = initial_context;
    queue_construct(&new_process->sending_queue);
    queue_construct(&new_process->message_queue);
    kernel.process_table = avl_insert(kernel.process_table, new_process->pid, new_process);
    priorityqueue_insert(&kernel.priority_queue, new_process, new_process->priority);
    kernel.next_pid++;
    return new_process->pid;
}

struct process_context_t *next_process()
{  
    if(kernel.active_process != NULL)
    {
        priorityqueue_insert(&kernel.priority_queue, kernel.active_process, kernel.active_process->priority);
    }
    kernel.active_process = priorityqueue_extract_min(&kernel.priority_queue);
    if(kernel.active_process != NULL)
    {
        paging_load_address_space(kernel.active_process->page_table);
        printf("entering process %08x cr3=%08x ctx=%08x.\n", kernel.active_process->pid, kernel.active_process->page_table, kernel.active_process->ctx);
        return kernel.active_process->ctx;
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
    priorityqueue_remove(&kernel.priority_queue, process);
    destroy_context(process->ctx);
    kfree(process);
    return S_OK;
}

int store_active_context(struct process_context_t *context, size_t size)
{
    if(kernel.active_process != NULL && kernel.active_process->ctx != NULL)
    {
        memcpy(kernel.active_process->ctx, context, size);
        return S_OK;
    }
    else
    {
        return S_DOESNT_EXIST;
    }
}

int open_port(unsigned long id)
{
    if(avl_get(kernel.port_table, id) != NULL)
    {
        return S_EXISTS;
    }
    printf("opening port %i -> %i\n", id, kernel.active_process->pid);
    struct port_t *port = kmalloc(sizeof(struct port_t));
    port->id = id;
    port->owner_pid = kernel.active_process->pid;
    kernel.port_table = avl_insert(kernel.port_table, id, port);
    return S_OK;
}

int close_port(unsigned long id)
{
    struct port_t *port = avl_get(kernel.port_table, id);
    if(port == NULL)
    {
        return S_DOESNT_EXIST;
    }
    else if(port->owner_pid != kernel.active_process->pid)
    {
        return S_INVALID_ARGUMENT;
    }
    printf("closing port %i attached to %i\n", id, kernel.active_process->pid);
    kernel.port_table = avl_remove(kernel.port_table, id);
    kfree(port);
    return S_OK;
}

int send_message(int recipient, struct message_t *message, int flags)
{
    int op_type = flags & IO_OP;
    int dest_type = flags & IO_RECIPIENT_TYPE;
    if((flags & ~(IO_OP | IO_RECIPIENT_TYPE)) != 0 || dest_type >= IO_MAILBOX)
    {
        printf("Invalid flags on send_message\n");
        return S_INVALID_ARGUMENT;
    }
    if(dest_type == IO_PORT)
    {
        struct port_t *port = avl_get(kernel.port_table, recipient);
        if(port != NULL)
        {
            recipient = port->owner_pid;
        }
        else
        {
            printf("Port %i does not exist\n", recipient);
            return S_DOESNT_EXIST;
        }
    }
    struct process_t *dest = avl_get(kernel.process_table, recipient);
    if(dest == NULL)
    {
        return S_DOESNT_EXIST;
    }
    else if(dest->message_buffer != NULL)
    {
        printf("Sending message directly from %i to %i\n", kernel.active_process->pid, dest->pid);
        struct message_t kernel_buffer;
        memcpy(&kernel_buffer, message, sizeof(struct message_t));
        kernel_buffer.sender = kernel.active_process->pid;
        paging_load_address_space(dest->page_table);
        memcpy(dest->message_buffer, &kernel_buffer, sizeof(struct message_t));
        paging_load_address_space(kernel.active_process->page_table);
        dest->message_buffer = NULL;
        dest->state = PROCESS_ACTIVE;
        set_context_return(dest->ctx, S_OK);
        priorityqueue_insert(&kernel.priority_queue, dest, dest->priority);
        return S_OK;
    }
    else if(op_type == IO_ASYNC)
    {
        printf("Queueing message from %i to %i\n", kernel.active_process->pid, dest->pid);
        struct message_t *queued_msg = kmalloc(sizeof(struct message_t));
        if(queued_msg == NULL)
        {
            return S_OUT_OF_MEMORY;
        }
        memcpy(queued_msg, message, sizeof(struct message_t));
        queue_insert(&dest->message_queue, queued_msg);
        return S_OK;
    }
    else
    {
        printf("Queueing process %i to %i\n", kernel.active_process->pid, dest->pid);
        queue_insert(&dest->sending_queue, kernel.active_process);
        kernel.active_process->state = PROCESS_SENDING;
        kernel.active_process = NULL;
        load_context(next_process());
    }
}

int receive_message(struct message_t *buffer, int flags)
{
    if(kernel.active_process->sending_queue.count > 0)
    {
        struct message_t kernel_buffer;
        struct process_t *sender = queue_get_next(&kernel.active_process->sending_queue);
        paging_load_address_space(sender->page_table);
        memcpy(&kernel_buffer, &sender->message_buffer, sizeof(struct message_t));
        kernel_buffer.sender = sender->pid;
        paging_load_address_space(kernel.active_process->page_table);
        memcpy(buffer, &kernel_buffer, sizeof(struct message_t));
        sender->state = PROCESS_ACTIVE;
        set_context_return(sender->ctx, S_OK);
        priorityqueue_insert(&kernel.priority_queue, sender, sender->priority);
        return S_OK;
    }
    else if(kernel.active_process->message_queue.count > 0)
    {
        struct message_t *queued_msg = queue_get_next(&kernel.active_process->message_queue);
        memcpy(buffer, queued_msg, sizeof(struct message_t));
        kfree(queued_msg);
        return S_OK;
    }
    else if((flags & IO_OP) == IO_ASYNC)
    {
        return S_DOESNT_EXIST;
    }
    else
    {
        kernel.active_process->message_buffer = buffer;
        kernel.active_process->state = PROCESS_REQUESTING;
        kernel.active_process = NULL;
        load_context(next_process());
    }
}

void panic(const char *message)
{
    printf("panic: %s", message);
    asm("cli");
    while(1) asm("hlt");
}

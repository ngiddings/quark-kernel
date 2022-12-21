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
#include "types/syscallid.h"

struct kernel_t kernel;

void kernel_initialize(struct boot_info_t *boot_info)
{
    insert_region(&boot_info->map, (physaddr_t)&_kernel_pstart, (physaddr_t)&_kernel_pend - (physaddr_t)&_kernel_pstart, M_UNAVAILABLE);
    initialize_page_map(&boot_info->map, (physaddr_t*)&_kernel_end, boot_info->memory_size, page_size);
    kminit(&_kernel_start, page_map_end(), 0xFFC00000 - (size_t)&_kernel_start, 64);
    initialize_screen();
    printf("***%s***\n", PACKAGE_STRING);
    printf("Total memory: %08x\n", boot_info->memory_size);
    printf("Type\t\tLocation\t\tSize\n");
    for (size_t i = 0; i < boot_info->map.size && boot_info->map.array[i].size > 0; i++)
    {
        printf("%i\t\t\t%08x\t\t%u\n", boot_info->map.array[i].type, boot_info->map.array[i].location, boot_info->map.array[i].size);
    }

    kernel.active_process = NULL;
    kernel.next_pid = 1;
    kernel.process_table = NULL;
    kernel.port_table = NULL;
    if(construct_priority_queue(&kernel.priority_queue, 512) != ENONE)
    {
        panic("Failed to construct priority queue.");
    }
    memset(kernel.syscall_table, 0, sizeof(struct syscall_t) * MAX_SYSCALL_ID);
    set_syscall(SYSCALL_TEST, 1, 0, test_syscall);
    set_syscall(SYSCALL_MMAP, 3, 0, mmap);
    set_syscall(SYSCALL_MUNMAP, 2, 0, munmap);
    set_syscall(SYSCALL_SEND, 3, 0, send);
    set_syscall(SYSCALL_RECEIVE, 2, 0, receive);
    set_syscall(SYSCALL_OPEN_PORT, 1, 0, open_port);
    set_syscall(SYSCALL_CLOSE_PORT, 1, 0, close_port);
    for(int i = 0; i < boot_info->module_count; i++)
    {
        if(kernel_load_module(&boot_info->modules[i]) != ENONE)
        {
            panic("Failed to load modules.");
        }
    }

    if(initialize_interrupts() != ENONE)
    {
        panic("Failed to initialize interrupts.");
    }

    irq_enable();
    load_context(kernel_advance_scheduler());
}

enum error_t set_syscall(int id, int arg_count, int pid, void *func_ptr)
{
    if(id < 0 || id > MAX_SYSCALL_ID)
    {
        return EOUTOFBOUNDS;
    }
    else if(kernel.syscall_table[id].defined)
    {
        return EINVALIDARG;
    }
    else if(arg_count < 0 || arg_count > 3)
    {
        return EINVALIDARG;
    }
    else if(pid != 0 && avl_get(kernel.process_table, pid) == NULL)
    {
        return EDOESNTEXIST;
    }
    else if(func_ptr == NULL)
    {
        return ENULLPTR;
    }
    kernel.syscall_table[id].defined = true;
    kernel.syscall_table[id].arg_count = arg_count;
    kernel.syscall_table[id].process_id = pid;
    kernel.syscall_table[id].func_ptr_0 = func_ptr;
    return ENONE;
}

size_t do_syscall(enum syscall_id_t id, syscall_arg_t arg1, syscall_arg_t arg2, syscall_arg_t arg3, void *pc, void *stack, unsigned long flags)
{
    if(id < 0 || id > MAX_SYSCALL_ID)
    {
        return ENOSYSCALL;
    }
    else if(!kernel.syscall_table[id].defined)
    {
        return ENOSYSCALL;
    }
    bool switched_address_space = false;
    if(kernel.syscall_table[id].process_id > 0)
    {
        struct process_t *callee = avl_get(kernel.process_table, kernel.syscall_table[id].process_id);
        if(callee == NULL)
        {
            kernel.syscall_table[id].defined = false;
            return ENOSYSCALL;
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

enum error_t kernel_load_module(struct module_t *module)
{
    physaddr_t module_address_space = create_address_space();
    if(module_address_space == ENOMEM) {
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
        case ENOMEM:
            panic("ran out of memory while mapping module");
        case EOUTOFBOUNDS:
            panic("got out-of-bounds error while mapping module");
        }
        load_offset += page_size;

    }
    int status = load_program(load_base);
    switch(status)
    {
    case ENOMEM:
        panic("ran out of memory while reading ELF file");
    case EOUTOFBOUNDS:
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
        case ENOMEM:
            panic("ran out of memory while unmapping module");
        case EOUTOFBOUNDS:
            panic("got out-of-bounds error while unmapping module");
        }
        load_offset += page_size;
    }
    if(kernel_spawn_process(module_entry, 1, current_address_space()) > 0)
    {
        return ENONE;
    }
    else
    {
        return -1;
    }
}

unsigned long kernel_current_pid()
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

struct process_context_t *kernel_current_context()
{
    if(kernel.active_process == NULL)
    {
        return NULL;
    }
    else
    {
        return kernel.active_process->ctx;
    }  
}

unsigned long kernel_spawn_process(void *program_entry, int priority, physaddr_t address_space)
{
    struct process_t *new_process = (struct process_t*) kmalloc(sizeof(struct process_t));
    if(new_process == NULL)
    {
        return 0;
    }
    struct process_context_t *initial_context = kmalloc(sizeof(struct process_context_t));
    if(initial_context == NULL)
    {
        return 0;
    }
    physaddr_t stack_page = reserve_page();
    if(stack_page % page_size)
    {
        return 0;
    }
    map_page((void*)&_kernel_start - page_size, stack_page, PAGE_PRESENT | PAGE_USERMODE | PAGE_RW);
    memset(initial_context, 0, sizeof(struct process_context_t));
    set_context_pc(initial_context, program_entry);
    set_context_flags(initial_context, DEFAULT_FLAGS);
    set_context_stack(initial_context, &_kernel_start);
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

struct process_context_t *kernel_advance_scheduler()
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

enum error_t kernel_terminate_process(size_t process_id)
{
    struct process_t *process = avl_get(kernel.process_table, process_id);
    if(process == NULL)
    {
        return EDOESNTEXIST;
    }
    if(kernel.active_process == process)
    {
        kernel.active_process = NULL;
    }
    kernel.process_table = avl_remove(kernel.process_table, process_id);
    priorityqueue_remove(&kernel.priority_queue, process);
    for(struct message_t *msg = queue_get_next(&process->message_queue); msg != NULL; msg = queue_get_next(&process->message_queue))
    {
        kfree(msg);
    }
    for(struct process_t *sender = queue_get_next(&process->sending_queue); sender != NULL; sender = queue_get_next(&process->sending_queue))
    {
        sender->state = PROCESS_ACTIVE;
        set_context_return(sender->ctx, EEXITED);
        priorityqueue_insert(&kernel.priority_queue, sender, sender->priority);
    }
    kfree(process->ctx);
    kfree(process);
    return ENONE;
}

enum error_t kernel_store_active_context(struct process_context_t *context)
{
    if(kernel.active_process != NULL && kernel.active_process->ctx != NULL)
    {
        memcpy(kernel.active_process->ctx, context, sizeof(*context));
        return ENONE;
    }
    else
    {
        return EDOESNTEXIST;
    }
}

enum error_t kernel_create_port(unsigned long id)
{
    if(avl_get(kernel.port_table, id) != NULL)
    {
        return EEXISTS;
    }
    printf("opening port %i -> %i\n", id, kernel.active_process->pid);
    struct port_t *port = kmalloc(sizeof(struct port_t));
    port->id = id;
    port->owner_pid = kernel.active_process->pid;
    kernel.port_table = avl_insert(kernel.port_table, id, port);
    return ENONE;
}

enum error_t kernel_remove_port(unsigned long id)
{
    struct port_t *port = avl_get(kernel.port_table, id);
    if(port == NULL)
    {
        return EDOESNTEXIST;
    }
    else if(port->owner_pid != kernel.active_process->pid)
    {
        return EPERM;
    }
    printf("closing port %i attached to %i\n", id, kernel.active_process->pid);
    kernel.port_table = avl_remove(kernel.port_table, id);
    kfree(port);
    return ENONE;
}

unsigned long kernel_get_port_owner(unsigned long id)
{
    struct port_t *port = avl_get(kernel.port_table, id);
    if(port == NULL)
    {
        return 0;
    }
    else
    {
        return port->owner_pid;
    }
}

enum error_t kernel_send_message(unsigned long recipient, struct message_t *message)
{
    struct process_t *dest = avl_get(kernel.process_table, recipient);
    if(dest == NULL)
    {
        return EDOESNTEXIST;
    }
    else if(dest->message_buffer != NULL && dest->state == PROCESS_REQUESTING)
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
        set_context_return(dest->ctx, ENONE);
        priorityqueue_insert(&kernel.priority_queue, dest, dest->priority);
        return ENONE;
    }
    else
    {
        return EBUSY;
    }
}

enum error_t kernel_queue_message(unsigned long recipient, struct message_t *message)
{
    struct process_t *dest = avl_get(kernel.process_table, recipient);
    if(dest != NULL)
    {
        printf("Queueing message from %i to %i\n", kernel.active_process->pid, dest->pid);
        struct message_t *queued_msg = kmalloc(sizeof(struct message_t));
        if(queued_msg == NULL)
        {
            return ENOMEM;
        }
        memcpy(queued_msg, message, sizeof(struct message_t));
        queue_insert(&dest->message_queue, queued_msg);
        return ENONE;
    }
    else
    {
        return EDOESNTEXIST;
    }
}

int receive_message(struct message_t *buffer, int flags)
{
    if(kernel.active_process->message_queue.count > 0)
    {
        struct message_t *queued_msg = queue_get_next(&kernel.active_process->message_queue);
        memcpy(buffer, queued_msg, sizeof(struct message_t));
        kfree(queued_msg);
        return ENONE;
    }
    else if((flags & IO_OP) == IO_ASYNC)
    {
        return EDOESNTEXIST;
    }
    else
    {
        kernel.active_process->message_buffer = buffer;
        kernel.active_process->state = PROCESS_REQUESTING;
        kernel.active_process = NULL;
        load_context(kernel_advance_scheduler());
    }
}

enum error_t kernel_register_interrupt_handler(unsigned long interrupt, signal_handler_t handler, void *userdata)
{
    if(avl_get(kernel.interrupt_handlers, interrupt) != NULL)
    {
        return EEXISTS;
    }
    struct signal_action_t *action = kmalloc(sizeof(struct signal_action_t));
    action->pid = kernel.active_process->pid;
    action->func_ptr = handler;
    action->userdata = userdata;
    kernel.interrupt_handlers = avl_insert(kernel.interrupt_handlers, interrupt, action);
    return ENONE;
}

enum error_t kernel_remove_interrupt_handler(unsigned long interrupt)
{
    if(avl_get(kernel.interrupt_handlers, interrupt) == NULL)
    {
        return EDOESNTEXIST;
    }
    kernel.interrupt_handlers = avl_remove(kernel.interrupt_handlers, interrupt);
    return ENONE;
}

enum error_t kernel_execute_interrupt_handler(unsigned long interrupt)
{
    struct signal_action_t *action = avl_get(kernel.interrupt_handlers, interrupt);
    if(action == NULL)
    {
        return EDOESNTEXIST;
    }

    struct process_t *process = avl_get(kernel.process_table, action->pid);
    if(process == NULL)
    {
        kernel.interrupt_handlers = avl_remove(kernel.interrupt_handlers, interrupt);
        return EDOESNTEXIST;
    }

    paging_load_address_space(process->page_table);

    struct signal_context_t siginfo = {
        .signal_id = interrupt
    };
    void *siginfo_ptr = context_stack_push_struct(process->ctx, &siginfo, sizeof(siginfo));
    context_stack_push_struct(process->ctx, process->ctx, sizeof(*process->ctx));
    context_stack_push(process->ctx, process->state);
    context_call_func(process->ctx, action->func_ptr, action->trampoline_ptr, 2, action->userdata, siginfo_ptr);
    if(process->state != PROCESS_ACTIVE)
    {
        process->state = PROCESS_ACTIVE;
        priorityqueue_insert(&kernel.priority_queue, process, process->priority);
    }

    paging_load_address_space(kernel.active_process->page_table);

    return ENONE;
}

enum error_t kernel_signal_return()
{
    context_cleanup_func(kernel.active_process->ctx, 2);
    context_stack_pop(kernel.active_process->ctx, &kernel.active_process->state);
    context_stack_pop_struct(kernel.active_process->ctx, kernel.active_process->ctx, sizeof(*kernel.active_process->ctx));
    if(kernel.active_process->state == PROCESS_REQUESTING)
    {
        receive_message(kernel.active_process->message_buffer, 0);
        load_context(kernel.active_process->ctx);
    }
    return ENONE;
}

void panic(const char *message)
{
    printf("panic: %s", message);
    asm("cli");
    while(1) asm("hlt");
}

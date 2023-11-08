#include "kernel.h"
#include "mmgr.h"
#include "heap.h"
#include "stdio.h"
#include "elf.h"
#include "syscalls.h"
#include "string.h"
#include "config.h"
#include "system.h"
#include "process.h"
#include "platform/interrupts.h"
#include "platform/context.h"
#include "platform/putc.h"
#include "types/status.h"
#include "types/syscallid.h"
#include "types/sigaction.h"

void *syscall_table[] = {
    NULL,
    (void*)test_syscall,
    (void*)syscall_map_anon,
    (void*)syscall_unmap_anon,
    (void*)syscall_map_physical,
    (void*)syscall_unmap_physical,
    (void*)syscall_open_port,
    (void*)syscall_close_port,
    (void*)syscall_send_pid,
    (void*)syscall_receive,
    (void*)syscall_create_object,
    (void*)syscall_aquire_object,
    (void*)syscall_release_object,
    (void*)syscall_get_pid,
    (void*)syscall_clone,
    (void*)syscall_signal_action,
    (void*)syscall_signal_return,
    (void*)syscall_signal_raise
};

struct shared_object_t
{
    physaddr_t phys_addr;
    size_t size;
    unsigned long access_flags;
    unsigned long refcount;
    struct avltree_t *users;
};

struct shared_object_mapping_t
{
    pid_t pid;
    oid_t oid;
    void *virt_addr;
};

struct port_t
{
    unsigned long id;
    pid_t owner_pid;
};

struct signal_context_t
{
    unsigned long signal_id;
};

struct kernel_t
{
    struct syscall_t syscall_table[MAX_SYSCALL_ID];
    struct priority_queue_t priority_queue;
    struct avltree_t *interrupt_handlers;
    struct avltree_t *port_table;
    struct avltree_t *object_table;
    struct avltree_t *process_table;
    struct process_t *active_process;
    pid_t next_pid;
    oid_t next_oid;
} kernel;

void kernel_initialize(struct boot_info_t *boot_info)
{
    if(initialize_screen())
    {
        asm("hlt");
    }
    printf("***%s***\n", PACKAGE_STRING);
    printf("Total memory: %08x\n", boot_info->memory_size);
    printf("kernel: %08x ... %08x\n", &_kernel_pstart, &_kernel_pend);
    printf("Type\t\tLocation\t\tSize\n");
    for (size_t i = 0; i < boot_info->map.size && boot_info->map.array[i].size > 0; i++)
    {
        printf("%i\t\t\t%08x\t\t%u\n", boot_info->map.array[i].type, boot_info->map.array[i].location, boot_info->map.array[i].size);
    }

    for(void *p = &_kernel_start; p < (void*)&_kernel_tend; p += page_size)
    {
        set_pte_type(p, page_table_levels - 1, PAGE_PRESENT);
    }

    memmap_insert_region(&boot_info->map, (physaddr_t)&_kernel_pstart, (physaddr_t)&_kernel_pend - (physaddr_t)&_kernel_pstart, M_UNAVAILABLE);
    if(initialize_page_map(&boot_info->map, (physaddr_t*)&_kernel_end, boot_info->memory_size, page_size))
    {
        kernel_panic("Failed to initialize page allocator.");
    }

    if(kminit(page_map_end(), 0xFFC00000 - (size_t)page_map_end()))
    {
        kernel_panic("Failed to initialize heap.");
    }
    
    kernel.active_process = NULL;
    kernel.next_pid = 1;
    kernel.next_oid = 1;
    kernel.process_table = NULL;
    kernel.port_table = NULL;
    kernel.object_table = NULL;
    if(construct_priority_queue(&kernel.priority_queue, 512) != ENONE)
    {
        kernel_panic("Failed to construct priority queue.");
    }
    memset(kernel.syscall_table, 0, sizeof(struct syscall_t) * MAX_SYSCALL_ID);
    kernel_set_syscall(SYSCALL_TEST, 1, test_syscall);
    kernel_set_syscall(SYSCALL_MAP_ANON, 3, syscall_map_anon);
    kernel_set_syscall(SYSCALL_UNMAP_ANON, 2, syscall_unmap_anon);
    kernel_set_syscall(SYSCALL_MAP_PHYS, 3, syscall_map_physical);
    kernel_set_syscall(SYSCALL_UNMAP_PHYS, 2, syscall_unmap_physical);
    kernel_set_syscall(SYSCALL_OPEN_PORT, 1, syscall_open_port);
    kernel_set_syscall(SYSCALL_CLOSE_PORT, 1, syscall_close_port);
    kernel_set_syscall(SYSCALL_SEND_PID, 3, syscall_send_pid);
    kernel_set_syscall(SYSCALL_SEND_PORT, 3, syscall_send_port);
    kernel_set_syscall(SYSCALL_RECEIVE, 2, syscall_receive);
    kernel_set_syscall(SYSCALL_CREATE_OBJECT, 3, syscall_create_object);
    kernel_set_syscall(SYSCALL_AQUIRE_OBJECT, 2, syscall_aquire_object);
    kernel_set_syscall(SYSCALL_RELEASE_OBJECT, 1, syscall_release_object);
    kernel_set_syscall(SYSCALL_GET_PID, 0, syscall_get_pid);
    kernel_set_syscall(SYSCALL_SIGNAL_ACTION, 3, syscall_signal_action);
    kernel_set_syscall(SYSCALL_SIGNAL_RETURN, 0, syscall_signal_return);
    kernel_set_syscall(SYSCALL_SIGNAL_RAISE, 2, syscall_signal_raise);
    kernel_set_syscall(SYSCALL_INTR_ACTION, 3, syscall_intr_action);
    kernel_set_syscall(SYSCALL_INTR_RETURN, 0, syscall_intr_return);
    for(int i = 0; i < boot_info->module_count; i++)
    {
        if(kernel_load_module(&boot_info->modules[i]) != ENONE)
        {
            kernel_panic("Failed to load modules.");
        }
    }

    if(initialize_interrupts() != ENONE)
    {
        kernel_panic("Failed to initialize interrupts.");
    }

    irq_enable();
    load_context(kernel_advance_scheduler());
}

error_t kernel_set_syscall(int id, int arg_count, void *func_ptr)
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
    else if(func_ptr == NULL)
    {
        return ENULLPTR;
    }
    kernel.syscall_table[id].defined = true;
    kernel.syscall_table[id].arg_count = arg_count;
    kernel.syscall_table[id].func_ptr_0 = func_ptr;
    return ENONE;
}

size_t kernel_do_syscall(syscall_id_t id, syscall_arg_t arg1, syscall_arg_t arg2, syscall_arg_t arg3, void *pc, void *stack, unsigned long flags)
{
    if(id < 0 || id > MAX_SYSCALL_ID)
    {
        return ENOSYSCALL;
    }
    else if(!kernel.syscall_table[id].defined)
    {
        return ENOSYSCALL;
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
    return result;
}

error_t kernel_load_module(struct module_t *module)
{
    physaddr_t module_address_space = create_address_space();
    if(module_address_space == ENOMEM) {
        kernel_panic("failed to create address space for module: out of memory");
    }
    paging_load_address_space(module_address_space);
    void *const load_base = (void*)0x80000000;
    physaddr_t p = module->start & ~(page_size - 1);
    map_region(load_base, p, module->end - p, PAGE_RW);
    int status = load_program(load_base);
    switch(status)
    {
    case ENOMEM:
        kernel_panic("ran out of memory while reading ELF file");
    case EOUTOFBOUNDS:
        kernel_panic("got out-of-bounds error while reading ELF file");
    }
    void *module_entry = ((struct elf_file_header_t*)load_base)->entry;
    printf("loaded module with entry point %08x\n", (unsigned int)module_entry);
    size_t load_offset = 0;
    for(physaddr_t p = module->start & ~(page_size - 1); p < module->end; p += page_size)
    {
        int status = unmap_page(load_base + load_offset);
        switch(status)
        {
        case ENOMEM:
            kernel_panic("ran out of memory while unmapping module");
        case EOUTOFBOUNDS:
            kernel_panic("got out-of-bounds error while unmapping module");
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

pid_t kernel_current_pid()
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

pid_t kernel_spawn_process(void *program_entry, int priority, physaddr_t address_space)
{
    physaddr_t stack_page = reserve_page();
    if(stack_page % page_size)
    {
        return 0;
    }

    map_page((void*)&_kernel_start - page_size, stack_page, PAGE_PRESENT | PAGE_USERMODE | PAGE_RW);
    struct process_t *new_process = process_construct(kernel.next_pid, &_kernel_start, program_entry, priority, address_space);
    if(new_process == NULL)
    {
        free_page(stack_page);
        return 0;
    }
    
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
        printf("entering process %08x cr3=%08x ctx=%08x sched=%i.\n", kernel.active_process->pid, kernel.active_process->page_table, kernel.active_process->ctx, kernel.priority_queue.size);
        return kernel.active_process->ctx;
    }
    kernel_panic("no processes available to enter!");
}

error_t kernel_terminate_process(pid_t pid)
{
    struct process_t *process = avl_get(kernel.process_table, pid);
    if(process == NULL)
    {
        return EDOESNTEXIST;
    }
    if(kernel.active_process == process)
    {
        kernel.active_process = NULL;
    }
    kernel.process_table = avl_remove(kernel.process_table, pid);
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

error_t kernel_store_active_context(struct process_context_t *context)
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

error_t kernel_create_port(unsigned long id)
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

error_t kernel_remove_port(unsigned long id)
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

pid_t kernel_get_port_owner(unsigned long id)
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

error_t kernel_send_message(unsigned long recipient, struct message_t *message)
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

error_t kernel_queue_message(unsigned long recipient, struct message_t *message)
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

int kernel_receive_message(struct message_t *buffer, int flags)
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

error_t kernel_register_interrupt_handler(unsigned long interrupt, signal_handler_t handler, void *userdata)
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

error_t kernel_remove_interrupt_handler(unsigned long interrupt)
{
    struct signal_action_t *action = avl_get(kernel.interrupt_handlers, interrupt);
    if(action == NULL)
    {
        return EDOESNTEXIST;
    }
    kfree(action);
    kernel.interrupt_handlers = avl_remove(kernel.interrupt_handlers, interrupt);
    return ENONE;
}

error_t kernel_execute_interrupt_handler(unsigned long interrupt)
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

error_t kernel_signal_return()
{
    context_cleanup_func(kernel.active_process->ctx, 2);
    context_stack_pop(kernel.active_process->ctx, &kernel.active_process->state);
    context_stack_pop_struct(kernel.active_process->ctx, kernel.active_process->ctx, sizeof(*kernel.active_process->ctx));
    if(kernel.active_process->state == PROCESS_REQUESTING)
    {
        kernel_receive_message(kernel.active_process->message_buffer, 0);
        load_context(kernel.active_process->ctx);
    }
    return ENONE;
}

error_t kernel_create_object(size_t size, unsigned long flags, oid_t *id)
{
    physaddr_t phys_addr = reserve_pages(size);
    if(phys_addr == ENOMEM)
    {
        return ENOMEM;
    }

    struct shared_object_t *obj = kmalloc(sizeof(struct shared_object_t));
    if(obj == NULL)
    {
        free_pages(phys_addr);
        return ENOMEM;
    }

    obj->phys_addr = phys_addr;
    obj->size = size;
    obj->access_flags = flags;
    obj->refcount = 0;
    obj->users = NULL;
    kernel.object_table = avl_insert(kernel.object_table, kernel.next_oid, obj);
    *id = kernel.next_oid;
    kernel.next_oid++;
    if(kernel.next_oid <= 0)
    {
        kernel.next_oid = 1;
    }
    return ENONE;
}

error_t kernel_attach_object(oid_t id, void *virt_addr)
{
    
}

error_t kernel_release_object(oid_t id)
{

}

void kernel_panic(const char *message)
{
    printf("panic: %s", message);
    asm("cli");
    while(1) asm("hlt");
}

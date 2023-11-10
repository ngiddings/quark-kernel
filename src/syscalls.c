#include "syscalls.h"
#include "kernel.h"
#include "mmgr.h"
#include "stdio.h"
#include "platform/context.h"
#include "types/status.h"

int test_syscall(syscall_arg_t str)
{
    if(str.ptr != NULL)
    {
        printf("%s", (char*)str.ptr);
    }
    return 17;
}

int syscall_map_anon(syscall_arg_t arg_location, syscall_arg_t arg_length, syscall_arg_t arg_flags)
{
    unsigned long location = arg_location.unsigned_int;
    unsigned long length = arg_length.unsigned_int;
    unsigned long flags = arg_flags.unsigned_int;
    if(location % page_size != 0 || length % page_size != 0)
    {
        return EINVALIDARG;
    }
    else if(location == (unsigned long)NULL)
    {
        return ENULLPTR;
    }
    
    for(size_t i = 0; i < length; i += page_size)
    {
        if(page_type((void*)(location + i)) & PAGE_PRESENT)
        {
            return EEXISTS;
        }
    }

    physaddr_t frame = reserve_pages(length);
    if(frame % page_size != 0)
    {
        return frame % page_size;
    }

    error_t status = map_region(location, frame, length, PAGE_USERMODE | PAGE_RW);
    if(status != ENONE)
    {
        kernel_panic("Unexpected failure to map virtual memory.");
    }
    return ENONE;
}

int syscall_unmap_anon(syscall_arg_t arg_location, syscall_arg_t arg_length)
{
    unsigned long location = arg_location.unsigned_int;
    unsigned long length = arg_length.unsigned_int;
    if(location % page_size != 0 || length % page_size != 0)
    {
        return EINVALIDARG;
    }
    else if(location == 0)
    {
        return ENULLPTR;
    }
    size_t n = 0;
    int status = ENONE;
    while(n < length && status == ENONE)
    {
        int type = page_type((void*)(location + n));
        physaddr_t frame;
        if(type & PAGE_PRESENT)
        {
            frame = unmap_page((void*)(location + n));
            if(type & PAGE_ANON)
            {
                status = free_page(frame);
            }
        }
    }
    return status;
}

int syscall_map_physical(syscall_arg_t arg_addr, syscall_arg_t arg_phys_addr, syscall_arg_t arg_length)
{
    void *addr = arg_addr.ptr;
    physaddr_t frame = arg_phys_addr.unsigned_int;
    unsigned long length = arg_length.unsigned_int;
    int status = ENONE;
    for(unsigned long offset = 0; offset < length; offset += page_size)
    {
        status = map_page(addr + offset, frame + offset, PAGE_USERMODE | PAGE_RW);
        if(status)
        {
            break;
        }
    }
    return status;
}

int syscall_unmap_physical(syscall_arg_t arg_addr, syscall_arg_t arg_length)
{
    void *addr = arg_addr.ptr;
    unsigned long length = arg_length.unsigned_int;
    int status = ENONE;
    for(unsigned long offset = 0; offset < length; offset += page_size)
    {
        status = unmap_page(addr + offset);
        if(status != ENONE)
        {
            break;
        }
    }
    return status;
}

int syscall_open_port(syscall_arg_t id)
{
    return kernel_create_port(id.unsigned_int);
}

int syscall_close_port(syscall_arg_t id)
{
    return kernel_remove_port(id.unsigned_int);
}

int syscall_send_pid(syscall_arg_t recipient, syscall_arg_t message, syscall_arg_t flags)
{
    unsigned long op_type = flags.unsigned_int & IO_OP;
    unsigned long dest_type = flags.unsigned_int & IO_RECIPIENT_TYPE;
    if((flags.unsigned_int & ~(IO_OP | IO_RECIPIENT_TYPE)) != 0 || dest_type >= IO_MAILBOX)
    {
        printf("Invalid flags on send()\n");
        return EINVALIDARG;
    }
    if(dest_type == IO_PORT)
    {
        recipient.unsigned_int = kernel_get_port_owner(recipient.unsigned_int);
        if(recipient.unsigned_int == 0)
        {
            return EDOESNTEXIST;
        }
    }
    error_t status = kernel_send_message(recipient.unsigned_int, message.ptr);
    if(status == EBUSY/* && op_type == IO_ASYNC*/)
    {
        return kernel_queue_message(recipient.unsigned_int, message.ptr);
    }
    else
    {
        return status;
    }
}

int syscall_send_port(syscall_arg_t recipient, syscall_arg_t message, syscall_arg_t flags)
{
    return ENOSYSCALL;
}

int syscall_receive(syscall_arg_t buffer, syscall_arg_t flags)
{
    return kernel_receive_message(buffer.ptr, flags.unsigned_int);
}

int syscall_create_object(void *location, size_t size, int flags)
{
    
    return ENOSYSCALL;
}

int syscall_aquire_object(oid_t id, void *location)
{
    return ENOSYSCALL;
}

int syscall_release_object(oid_t id)
{
    return ENOSYSCALL;
}

int syscall_get_pid()
{
    return kernel_current_pid();
}

int syscall_clone(void (*entry)(void*), void *arg, void *stack, int flags)
{
    return ENOSYSCALL;
}

int syscall_signal_action(int id, struct signal_action_t *action, int flags)
{
    return ENOSYSCALL;
}

int syscall_signal_return()
{
    return ENOSYSCALL;
}

int syscall_signal_raise(pid_t pid, int sigid)
{
    return ENOSYSCALL;
}

int syscall_intr_action(int id, struct signal_action_t *action, int flags)
{
    return ENOSYSCALL;
}

int syscall_intr_return()
{
    return ENOSYSCALL;
}

#include "syscalls.h"
#include "kernel.h"
#include "mmgr.h"
#include "stdio.h"
#include "platform/context.h"
#include "types/status.h"

size_t test_syscall(syscall_arg_t str)
{
    printf("%s", (char*)str.ptr);
    return 17;
}

size_t mmap(syscall_arg_t arg_location, syscall_arg_t arg_length, syscall_arg_t arg_flags)
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
    size_t n = 0;
    int status = ENONE;
    while(n < length && status == ENONE)
    {
        physaddr_t frame = reserve_page();
        status = frame % page_size;
        if(status == ENONE)
        {
            status = map_page((void*)(location + n), frame, PAGE_USERMODE | PAGE_RW);
            if(status != ENONE && free_page(frame) != ENONE)
            {
                panic("critical error reached during mmap.");
            }
            n += page_size;
        }
        else
        {
            break;
        }
    }
    if(status != ENONE && munmap(arg_location, arg_length) != ENONE)
    {
        panic("critical error reached during mmap.");
    }
    return status;
}

size_t munmap(syscall_arg_t arg_location, syscall_arg_t arg_length)
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

size_t terminate_self()
{
    return kernel_terminate_process(kernel_current_pid());
}

size_t send(syscall_arg_t recipient, syscall_arg_t message, syscall_arg_t flags)
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

size_t receive(syscall_arg_t buffer, syscall_arg_t flags)
{
    return receive_message(buffer.ptr, flags.unsigned_int);
}

size_t open_port(syscall_arg_t id)
{
    return kernel_create_port(id.unsigned_int);
}

size_t close_port(syscall_arg_t id)
{
    return kernel_remove_port(id.unsigned_int);
}

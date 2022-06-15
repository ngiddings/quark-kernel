#include "syscalls.h"
#include "kernel.h"
#include "mmgr.h"
#include "stdio.h"
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
        return S_INVALID_ARGUMENT;
    }
    else if(location == NULL)
    {
        return S_NULL_POINTER;
    }
    for(size_t i = 0; i < length; i += page_size)
    {
        if(page_type((void*)(location + i)) & PAGE_PRESENT)
        {
            return S_EXISTS;
        }
    }
    size_t n = 0;
    int status = S_OK;
    while(n < length && status == S_OK)
    {
        physaddr_t frame = reserve_page();
        status = frame % page_size;
        if(status == S_OK)
        {
            status = map_page((void*)(location + n), frame, PAGE_USERMODE | PAGE_RW);
            if(status != S_OK && free_page(frame) != S_OK)
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
    if(status != S_OK && munmap(arg_location, arg_length) != S_OK)
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
        return S_INVALID_ARGUMENT;
    }
    else if(location == 0)
    {
        return S_NULL_POINTER;
    }
    size_t n = 0;
    int status = S_OK;
    while(n < length && status == S_OK)
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
    return terminate_process(active_process());
}

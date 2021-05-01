#include "syscalls.h"
#include "kernel.h"
#include "mmgr.h"
#include "stdio.h"
#include "types/status.h"

size_t test_syscall(struct kernel_t *kernel, size_t arg1, size_t arg2, size_t arg3)
{
    printf("%s", (char*)arg1);
    return 17;
}

size_t mmap(struct kernel_t *kernel, size_t location, size_t length, size_t flags)
{
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
        if(page_type((void*)(location + i)) != PAGE_NOT_PRESENT)
        {
            return S_EXISTS;
        }
    }
    size_t n = 0;
    int status = S_OK;
    while(n < length && status == S_OK)
    {
        physaddr_t frame = reserve_page(kernel->page_stack);
        status = frame % page_size;
        if(status == S_OK)
        {
            status = map_page(kernel->page_stack, (void*)(location + n), frame, PAGE_USERMODE | PAGE_RW);
            if(status != S_OK && free_page(kernel->page_stack, frame) != S_OK)
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
    if(status != S_OK && munmap(kernel, location, page_size, NULL) != S_OK)
    {
        panic("critical error reached during mmap.");
    }
    return status;
}

size_t munmap(struct kernel_t *kernel, size_t location, size_t length, size_t arg3)
{
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
        enum page_type_t type = page_type((void*)(location + n));
        physaddr_t frame;
        if(type != PAGE_NOT_PRESENT)
        {
            frame = unmap_page((void*)(location + n));
        }
        if(type == PAGE_ANON)
        {
            status = free_page(kernel->page_stack, frame);
        }
    }
    return status;
}

size_t terminate_self(struct kernel_t *kernel, size_t arg1, size_t arg2, size_t arg3)
{
    return terminate_process(kernel, kernel->active_process->resource_id);
}

#include <stdbool.h>
#include <libmalloc/list_alloc.h>
#include "heap.h"
#include "mmgr.h"
#include "math.h"
#include "stdio.h"
#include "types/status.h"

list_alloc_descriptor_t system_heap;

static int mmap_callback(void *location, unsigned long size)
{
    size += (unsigned long)location % page_size;
    location -= (unsigned long)location % page_size;
    int status = ENONE;
    for(unsigned long i = 0; i < size; i += page_size)
    {
        if((page_type(location + i) & PAGE_PRESENT))
        {
            continue;
        }
        physaddr_t frame = reserve_page();
        if(frame == ENOMEM)
        {
            return ENOMEM;
        }
        else if((status = map_page(location + i, reserve_page(), PAGE_RW)))
        {
            return status;
        }
    }
    return status;
}

int kminit(void *base, size_t heap_size)
{
    memory_region_t map_array[8];
    memory_map_t map = {
        .array = map_array,
        .capacity = 8,
        .size = 0
    };
    memmap_insert_region(&map, base, heap_size, M_AVAILABLE);
    physaddr_t phys_addr = reserve_pages(heap_size);
    if(phys_addr == ENOMEM)
    {
        return ENOMEM;
    }
    for(unsigned long off = 0; off < heap_size; off += page_size)
    {
        if((page_type(base + off) & PAGE_PRESENT))
        {
            continue;
        }
        else if(map_page(base + off, phys_addr + off, PAGE_RW))
        {
            return ENOMEM;
        }
    }
    return list_alloc_init(&system_heap, &map);
}

void *kmalloc(size_t size)
{
    void *p = list_alloc_reserve(&system_heap, size);
    if(p == NOMEM)
    {
        return NULL;
    }
    else
    {
        return p;
    }
}

void kfree(void *ptr)
{
    list_alloc_free(&system_heap, (unsigned long)ptr);
}

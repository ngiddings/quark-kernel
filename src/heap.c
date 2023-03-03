#include <stdbool.h>
#include <libmalloc/bitmap_alloc.h>
#include "heap.h"
#include "mmgr.h"
#include "math.h"
#include "stdio.h"
#include "types/status.h"

bitmap_heap_descriptor_t system_heap;

static int map_block(void *location, unsigned long size)
{
    for(int n = 0; n < size; n += page_size)
    {
        if(!(page_type(location + n) & PAGE_PRESENT))
        {
            int status = map_page(location + n, reserve_page(), PAGE_RW);
            if(status)
            {
                return status;
            }
        }
    }
    return ENONE;
}

static int mmap_callback(void *location, unsigned long size)
{
    int status = ENONE;
    for(unsigned long i = 0; i < size; i += page_size)
    {
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

int kminit(void *base, void* start, size_t heap_size, size_t block_size)
{
    static unsigned long heap_cache[16];
    system_heap.bitmap = NULL;
    system_heap.cache = heap_cache;
    system_heap.cache_capacity = 16;
    system_heap.block_bits = 4;
    system_heap.block_size = block_size;
    system_heap.offset = (unsigned long)base;
    memory_region_t map_array[8];
    memory_map_t map = {
        .array = map_array,
        .capacity = 8,
        .size = 0
    };
    memmap_insert_region(&map, 0, heap_size, M_AVAILABLE);
    memmap_insert_region(&map, 0, start - base, M_UNAVAILABLE);
    return initialize_heap(&system_heap, &map, mmap_callback);
}

void *kmalloc(size_t size)
{
    unsigned long loc = reserve_region(&system_heap, size);
    if(loc == NOMEM)
    {
        return NULL;
    }
    else if(map_block((void*)loc, size))
    {
        return NULL;
    }
    else
    {
        return (void*)loc;
    }
}

void kfree(void *ptr)
{
    free_region(&system_heap, (unsigned long)ptr, 0);
}

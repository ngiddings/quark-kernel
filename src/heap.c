#include <stdbool.h>
#include <libmalloc/bitmap_alloc.h>
#include "heap.h"
#include "mmgr.h"
#include "math.h"
#include "stdio.h"
#include "types/status.h"

/*

#define AVAIL 0
#define UNAVAIL 1
#define ALLOCATED 2

struct heap_t
{
    void *base;
    struct heap_node_t *header;
    size_t heap_size;
    size_t block_size;
    size_t tree_height;
} system_heap;

struct heap_node_t
{
    size_t mapped : 1;
    size_t state : 2;
    size_t height : 5;
} __attribute__ ((packed));

size_t find_free_block(struct heap_t *heap, size_t height)
{
    if(height > heap->tree_height)
    {
        return 0;
    }
    for(size_t index = 1 << (heap->tree_height - height); index < 1 << (heap->tree_height - height + 1); index++)
    {
        if(heap->header[index].state == AVAIL)
        {
            return index;
        }
    }
    size_t index = find_free_block(heap, height + 1);
    if(index)
    {
        heap->header[index].state = UNAVAIL;
        heap->header[index << 1].state = AVAIL;
        heap->header[(index << 1) ^ 1].state = AVAIL;
    }
    return index << 1;
}

int map_region(struct heap_t *heap, size_t height, size_t index)
{
    int status = 0;
    if(heap->header[index].mapped == 0)
    {
        if(height > 0)
        {
            status = map_region(heap, height - 1, index << 1);
            if(status == 0)
            {
                status = map_region(heap, height - 1, (index << 1) ^ 1);
            }
        }
        else
        {
            void *ptr = (void*) ((size_t) heap->base + (heap->block_size << height) * (index - (1 << (heap->tree_height - height))));
            if((page_type(ptr) & PAGE_PRESENT) == 0)
            {
                status = map_page(ptr, reserve_page(), PAGE_RW);
            }
        }
        heap->header[index].mapped = 1;
    }
    return status;
}

int heap_contruct(struct heap_t *heap, void *base, void *start, size_t heap_size, size_t block_size)
{
    heap->base = base;
    heap->header = (struct heap_node_t*) start;
    heap->heap_size = heap_size;
    heap->block_size = block_size;
    heap->tree_height = llog2(heap_size / block_size);
    size_t header_size = (heap_size / block_size) << 1;
    for(size_t i = 1; i <= (heap_size / block_size) * 2; i++)
    {
        //printf("%i\n",i);
        int flags = page_type((void*) heap->header + i);
        if((flags & PAGE_PRESENT) == 0)
        {
            int status = map_page((void*)heap->header + i, reserve_page(), PAGE_RW);
            if(status != ENONE)
            {
                return status;
            }
        }
        heap->header[i].state = UNAVAIL;
        heap->header[i].mapped = 0;
    }
    for(size_t i = 0; i < heap_size / block_size; i++)
    {
        if(block_size * i >= header_size + (start - base))
        {
            size_t index = i + (1 << heap->tree_height);
            heap->header[index].state = AVAIL;
            for(; index > 1 && heap->header[index ^ 1].state == 0; index >>= 1)
            {
                heap->header[index].state = UNAVAIL;
                heap->header[index ^ 1].state = UNAVAIL;
                heap->header[index >> 1].state = AVAIL;
            }
        }
        else
        {
            heap->header[i + (1 << heap->tree_height)].state = UNAVAIL;
        }
    }
    return ENONE;
}

void *heap_allocate(struct heap_t *heap, size_t size)
{
    size += heap->block_size - 1;
    size -= size % heap->block_size;
    size_t height = llog2(size / heap->block_size);
    size_t index = find_free_block(heap, height);
    if(index)
    {
        heap->header[index].state = ALLOCATED;
        void *ptr = (void*) ((size_t) heap->base + (heap->block_size << height) * (index - (1 << (heap->tree_height - height))));
        map_region(heap, height, index);
        return ptr;
    }
    return NULL;
}

void heap_free(struct heap_t *heap, void *ptr)
{
    size_t offset = (size_t) ptr - (size_t) heap->base;
    size_t index = (offset / heap->block_size) + (1 << heap->tree_height);
    for(; index > 0 && heap->header[index].state == UNAVAIL; index >>= 1);
    heap->header[index].state = AVAIL;
    for(; index > 1 && heap->header[index ^ 1].state == AVAIL; index >>= 1)
    {
        heap->header[index].state = UNAVAIL;
        heap->header[index ^ 1].state = UNAVAIL;
        heap->header[index >> 1].state = AVAIL;
    }
}

*/

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

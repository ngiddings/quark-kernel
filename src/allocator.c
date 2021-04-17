#include "allocator.h"

struct linear_allocator_t
{
    void *bottom;
    void *top;
} allocator;

void initialize_allocator(void *bottom, void *top)
{
    allocator.bottom = bottom;
    allocator.top = top;
}

void *allocate_from_bottom(size_t size)
{
    if((size_t)allocator.bottom + size <= (size_t)allocator.top)
    {
        void *ptr = allocator.bottom;
        allocator.bottom += size;
        return ptr;
    }
    return (void*)NULL;
}

void *allocate_from_top(size_t size)
{
    if((size_t)allocator.top - size >= (size_t)allocator.bottom)
    {
        allocator.top -= size;
        return allocator.top;
    }
    return (void*)NULL;
}

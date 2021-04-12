#include "pageallocator.h"
#include "types/memorytype.h"
#include "types/status.h"

physaddr_t reserve_page(struct page_stack_t *stack)
{
    if(stack->stack_pointer < stack->base_pointer)
    {
        physaddr_t frame = *stack->stack_pointer;
        *stack->stack_pointer = (physaddr_t) 0;
        stack->stack_pointer++;
        return frame;
    }
    return S_OUT_OF_MEMORY;
}

int free_page(struct page_stack_t *stack, physaddr_t location)
{
    if(stack->stack_pointer > stack->limit_pointer)
    {
        stack->stack_pointer--;
        *stack->stack_pointer = location;
        return S_OK;
    }
    return S_OUT_OF_MEMORY;
}

size_t free_page_count(struct page_stack_t *stack)
{
    return stack->base_pointer - stack->stack_pointer;
}

int initialize_page_stack(struct page_stack_t *stack, struct memory_map_t *map, size_t page_size)
{
    stack->total_pages = 0;
    for(int i = 0; i < map->size; i++)
    {
        if(map->array[i].type != M_AVAILABLE)
        {
            continue;
        }
        size_t location = (map->array[i].location + page_size - 1) & ~(page_size - 1);
        while(location + page_size <= map->array[i].location + map->array[i].size)
        {
            if(free_page(stack, location) != S_OK)
            {
                return S_OUT_OF_MEMORY;
            }
            stack->total_pages++;
            location += page_size;
        }
    }
}

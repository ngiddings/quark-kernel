#include "pageallocator.h"
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
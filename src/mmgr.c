#include "mmgr.h"
#include "string.h"
#include "platform/paging.h"
#include "types/status.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Describes a stack containing the physical addresses of available page
 * frames.
 * 
 */
struct page_stack_t
{
    /**
     * @brief The total number of physical pages managed by the system.
     * 
     */
    size_t total_pages;

    /**
     * @brief Points to the topmost physical address on the stack.
     * 
     */
    physaddr_t *stack_pointer;

    /**
     * @brief Points to the bottom of the stack.
     * 
     */
    physaddr_t *base_pointer;

    /**
     * @brief Points to the limit of the stack. The stack cannot grow beyond
     * this point.
     * 
     */
    physaddr_t *limit_pointer;

} page_stack;

int initialize_page_stack(struct memory_map_t *map, physaddr_t *stack_base)
{
    page_stack.base_pointer = stack_base;
    page_stack.limit_pointer = stack_base;
    page_stack.stack_pointer = stack_base;
    page_stack.total_pages = 0;
    for(int i = 0; i < map->size; i++)
    {
        if(map->array[i].type != M_AVAILABLE)
        {
            continue;
        }
        size_t location = (map->array[i].location + page_size - 1) & ~(page_size - 1);
        while(location + page_size <= map->array[i].location + map->array[i].size)
        {
            if(free_page(location) != S_OK)
            {
                return S_OUT_OF_MEMORY;
            }
            page_stack.total_pages++;
            location += page_size;
        }
    }
    return S_OK;
}

physaddr_t reserve_page()
{
    if(page_stack.stack_pointer > page_stack.base_pointer)
    {
        page_stack.stack_pointer--;
        physaddr_t frame = *page_stack.stack_pointer;
        *page_stack.stack_pointer = (physaddr_t) 0;
        return frame;
    }
    return S_OUT_OF_MEMORY;
}

int free_page(physaddr_t location)
{
    if(page_stack.stack_pointer < page_stack.limit_pointer)
    {
        *page_stack.stack_pointer = location;
        page_stack.stack_pointer++;
        return S_OK;
    }
    else
    {
        switch(map_page(page_stack.limit_pointer, location, PAGE_RW))
        {
        case S_OUT_OF_MEMORY:
            return S_OUT_OF_MEMORY;
        case S_OUT_OF_BOUNDS:
            return S_OUT_OF_BOUNDS;
        case S_OK:
            page_stack.limit_pointer += page_size / sizeof(*page_stack.limit_pointer);
            return S_OK;
        }
        return S_OUT_OF_MEMORY;
    }
}

size_t free_page_count()
{
    return page_stack.base_pointer - page_stack.stack_pointer;
}

void *page_stack_bottom()
{
    return (void*)page_stack.base_pointer;
}

void *page_stack_top()
{
    return (void*)page_stack.limit_pointer;
}

physaddr_t create_address_space()
{
    physaddr_t table = reserve_page();
    int result;
    if (table == S_OUT_OF_MEMORY)
    {
        return S_OUT_OF_MEMORY;
    }
    else if((result = paging_init_top_table(table)))
    {
        return result;
    }
    else
    {
        return table;
    }
}

physaddr_t current_address_space()
{
    return paging_current_address_space();
}

int map_page(void *page, physaddr_t frame, int flags)
{
    if (frame % page_size != 0)
    {
        return S_INVALID_ARGUMENT;
    }
    for(int level = 0; level < page_table_levels - 1; level++)
    {
        int present = get_pte_type(page, level) & PAGE_PRESENT;
        if(present == 0)
        {
            physaddr_t new_table = reserve_page();
            if(new_table == S_OUT_OF_MEMORY)
            {
                return S_OUT_OF_MEMORY;
            }
            set_pte(page, level, PAGE_PRESENT | PAGE_USERMODE | PAGE_RW, new_table);
            wipe_page_table(page, level + 1);
        }
    }
    set_pte(page, page_table_levels - 1, PAGE_PRESENT | flags, frame);
    return S_OK;
}

physaddr_t unmap_page(void *page)
{
    for(int level = 0; level < page_table_levels; level++)
    {
        if((get_pte_type(page, level) & PAGE_PRESENT) == 0)
            return S_OUT_OF_BOUNDS;
    }
    physaddr_t frame = get_pte_address(page, page_table_levels - 1);
    set_pte(page, page_table_levels - 1, 0, 0);
    return frame;
}

int page_type(void *page)
{
    for(int level = 0; level < page_table_levels - 1; level++)
    {
        int flags = get_pte_type(page, level);
        if((flags & PAGE_PRESENT) == 0)
            return flags;
    }
    return get_pte_type(page, page_table_levels - 1);
}

physaddr_t physical_address(void *linear_address)
{
    return get_pte_address(linear_address, page_table_levels - 1);
}

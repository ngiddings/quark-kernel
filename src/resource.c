#include "resource.h"
#include "mmgr.h"
#include "allocator.h"
#include "types/status.h"

int get_free_resource_slot(struct resource_table_t *table, struct page_stack_t *page_stack)
{
    if(table == NULL)
    {
        return -1;
    }
    size_t capacity = table->limit - table->array;
    for(int i = 0; i < capacity; i++)
    {
        if(table->array[i].type == RESOURCE_UNAVAILABLE)
        {
            return i;
        }
    }
    void *new_limit = allocate_from_bottom(page_size);
    if(new_limit != NULL)
    {
        if(map_page(page_stack, new_limit, reserve_page(page_stack), PAGE_RW) != S_OK)
        {
            return -1;
        }
        table->limit = (struct resource_t*)(new_limit + page_size);
        return get_free_resource_slot(table, page_stack);
    }
    return -1;
}

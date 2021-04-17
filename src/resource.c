#include "resource.h"

int find_resource_slot(struct resource_table_t *table)
{
    for(int i = 0; i < table->capacity; i++)
    {
        if(table->array[i].type == RESOURCE_UNAVAILABLE)
        {
            return i;
        }
    }
    return -1;
}

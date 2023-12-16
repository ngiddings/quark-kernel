#include "addressspace.h"
#include "heap.h"
#include "mmgr.h"

address_space_t *address_space_construct()
{
    address_space_t *as = kmalloc(sizeof(address_space_t));
    if(as == NULL)
    {
        return NULL;
    }

    as->page_table = create_address_space();
    if(as->page_table == ENOMEM)
    {
        kfree(as);
        return NULL;
    }

    as->refcount = 0;
    return as;
}

void address_space_switch(address_space_t *address_space)
{
    paging_load_address_space(address_space->page_table);
}

address_space_t *address_space_release(address_space_t *address_space)
{
    address_space->refcount--;
    if(address_space->refcount <= 0)
    {
        address_space_destroy(address_space);
        return NULL;
    }
    return address_space;
}

void address_space_destroy(address_space_t *address_space)
{
    // TODO: Not implemented.
    kfree(address_space);
}

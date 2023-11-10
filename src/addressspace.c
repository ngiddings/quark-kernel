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
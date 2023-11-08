#include "addressspace.h"
#include "heap.h"
#include "mmgr.h"

addressspace_t *addressspace_construct()
{
    addressspace_t *as = kmalloc(sizeof(addressspace_t));
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
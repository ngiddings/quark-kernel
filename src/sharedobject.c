#include "sharedobject.h"
#include "avltree.h"
#include "mmgr.h"
#include "heap.h"

struct shared_object *create_shared_object(size_t size, unsigned long flags)
{
    physaddr_t phys_addr = reserve_pages(size);
    if(phys_addr == NULL)
    {
        return NULL;
    }

    struct shared_object_t *obj = kmalloc(sizeof(struct shared_object_t));
    if(obj == NULL)
    {
        free_pages(phys_addr);
        return NULL;
    }

    obj->phys_addr = phys_addr;
    obj->size = size;
    obj->access_flags = flags;
    obj->refcount = 0;
    return obj;
}

void destroy_shared_object(struct shared_object_t *obj)
{
    if(obj != NULL)
    {
        free_pages(obj->phys_addr);
        kfree(obj);
    }
}
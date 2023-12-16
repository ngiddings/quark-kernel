#ifndef QUARK_SHARED_OBJECT_H
#define QUARK_SHARED_OBJECT_H

#include "types/physaddr.h"
#include <stddef.h>

typedef struct shared_object_t
{
    physaddr_t phys_addr;
    size_t size;
    unsigned long access_flags;
    unsigned long refcount;
} shared_object_t;

struct shared_object *create_shared_object(size_t size, unsigned long flags);

void destroy_shared_object(struct shared_object_t *obj);

#endif
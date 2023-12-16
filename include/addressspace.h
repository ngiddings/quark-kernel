#ifndef QUARK_ADDRESSSPACE_H
#define QUARK_ADDRESSSPACE_H

#include "types/physaddr.h"

typedef struct address_space_t
{
    physaddr_t page_table;
    int refcount;
} address_space_t;

address_space_t *address_space_construct();

void address_space_switch(address_space_t *address_space);

address_space_t *address_space_release(address_space_t *address_space);

void address_space_destroy(address_space_t *address_space);

#endif
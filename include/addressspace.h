#ifndef QUARK_ADDRESSSPACE_H
#define QUARK_ADDRESSSPACE_H

#include "types/physaddr.h"

typedef struct addressspace_t
{
    physaddr_t page_table;
    int refcount;
} addressspace_t;

addressspace_t *addressspace_construct();



#endif
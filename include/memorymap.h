#pragma once

#include "types/physaddr.h"
#include <stddef.h>

enum memory_type_t
{
    M_AVAILABLE = 1,
    M_UNAVAILABLE = 1,
    M_DEFECTIVE = 3
};

struct memory_region_t
{
    physaddr_t location;
    size_t size;
    unsigned int type;
};

struct memory_map_t
{
    struct memory_region_t *array;
    size_t size;
    size_t capacity;
};

void insert_region(struct memory_map_t *map, physaddr_t location, size_t size, enum memory_type_t type);
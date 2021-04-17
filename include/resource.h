#pragma once

#include "process.h"
#include <stddef.h>

enum resource_type_t
{
    RESOURCE_UNAVAILABLE = 0,
    RESOURCE_PROCESS
};

struct resource_t
{
    size_t type;
    union
    {
        struct process_t process;
    };
};

struct resource_table_t
{
    struct resource_t *array;
    size_t capacity;
};

int find_resource_slot(struct resource_table_t *table);

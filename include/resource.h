#pragma once

#include "pageallocator.h"
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
    struct resource_t *limit;
};

int construct_resource_table(struct resource_table_t *table, struct page_stack_t *page_stack);

int get_free_resource_slot(struct resource_table_t *table, struct page_stack_t *page_stack);

#pragma once

#include <stddef.h>

struct process_state_t;

struct process_t
{
    size_t priority;

    struct process_state_t *state;
};
#pragma once

#include <stddef.h>

void initialize_allocator(void *bottom, void *top);

void *allocate_from_bottom(size_t size);

void *allocate_from_top(size_t size);
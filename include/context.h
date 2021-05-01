#pragma once

#include "pageallocator.h"
#include "kernel.h"

void *initialize_context(void *task_entry, struct page_stack_t *page_stack);

void save_context(struct process_context_t *context, void *ptr);

void load_context(struct process_context_t *context);

#pragma once

#include "pageallocator.h"
#include "process.h"

void *initialize_context(void *task_entry, struct page_stack_t *page_stack);

void save_context(struct process_state_t *context, void *ptr);

void load_context(struct process_state_t *context);

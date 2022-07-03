#pragma once

struct process_context_t;

void *initialize_context(void *task_entry);

void destroy_context(void *ctx);

void save_context(struct process_context_t *context);

void load_context(struct process_context_t *context);

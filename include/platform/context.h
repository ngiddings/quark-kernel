#pragma once

void *initialize_context(void *task_entry);

void destroy_context(void *ctx);

void save_context(struct process_context_t *context, void *ptr);

void load_context(struct process_context_t *context);

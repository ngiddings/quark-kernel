#pragma once

struct process_context_t;

/**
 * @brief Allocates a new process context and initializes it with the given 
 * program counter.
 * 
 * @param task_entry 
 * @return void* 
 */
void *initialize_context(void *pc);

void destroy_context(void *ctx);

void save_context(struct process_context_t *context);

void load_context(struct process_context_t *context);

void set_context_pc(struct process_context_t *context, void *pc);

void set_context_stack(struct process_context_t *context, void *stack);

void set_context_flags(struct process_context_t *context, unsigned long flags);

void set_context_return(struct process_context_t *context, unsigned long value);

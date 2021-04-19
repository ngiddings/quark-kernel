#pragma once

#include "pageallocator.h"
#include "priorityqueue.h"
#include "resource.h"
#include "module.h"
#include "process.h"
#include <stddef.h>

enum syscall_id_t
{
    SYSCALL_YIELD = 1
};

struct kernel_t
{
    struct page_stack_t *page_stack;
    struct priority_queue_t *priority_queue;
    struct process_t *active_process;
    struct resource_table_t *resource_table;
};

extern struct kernel_t kernel_state;

size_t do_syscall(struct kernel_t *kernel, enum syscall_id_t id, size_t arg1, size_t arg2, size_t arg3);

int load_module(struct kernel_t *kernel, struct module_t *module);

struct process_state_t *next_process(struct kernel_t *kernel, struct process_state_t *prev_state);

void panic(const char *message) __attribute__ ((noreturn));

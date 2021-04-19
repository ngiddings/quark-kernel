#pragma once

#include "pageallocator.h"
#include "priorityqueue.h"
#include "resource.h"
#include "module.h"
#include "process.h"
#include "memorymap.h"
#include <stddef.h>

enum syscall_id_t
{
    SYSCALL_TEST = 1,
    SYSCALL_YIELD,
    SYSCALL_MMAP,
    SYSCALL_MUNMAP
};

typedef size_t (*syscall_t)(struct kernel_t*, size_t, size_t, size_t);

struct kernel_t
{
    struct page_stack_t *page_stack;
    struct priority_queue_t *priority_queue;
    struct process_t *active_process;
    struct resource_table_t *resource_table;
};

extern syscall_t syscall_table[32];

extern struct kernel_t kernel_state;

void construct_kernel_state(struct kernel_t *kernel, struct page_stack_t *page_stack, 
    struct priority_queue_t *priority_queue, struct resource_table_t *resource_table,
    size_t module_count, struct module_t *module_list);

size_t do_syscall(struct kernel_t *kernel, enum syscall_id_t id, size_t arg1, size_t arg2, size_t arg3);

int load_module(struct kernel_t *kernel, struct module_t *module);

struct process_state_t *next_process(struct kernel_t *kernel, struct process_state_t *prev_state);

void panic(const char *message) __attribute__ ((noreturn));

#pragma once

#include "pageallocator.h"
#include "priorityqueue.h"
#include "resource.h"
#include "module.h"
#include "memorymap.h"
#include "syscallid.h"
#include <stddef.h>

typedef size_t (*syscall_t)(struct kernel_t*, size_t, size_t, size_t);

enum process_state_t
{
    PROCESS_ACTIVE,
    PROCESS_WAITING
};

struct process_context_t;

struct message_t
{
    uint16_t sender, type;
    uint32_t param1, param2, param3;
};

struct process_t
{
    size_t priority;
    size_t resource_id;
    physaddr_t page_table;
    struct process_context_t *state;
    struct message_t *message;
};

struct kernel_t
{
    struct page_stack_t *page_stack;
    struct priority_queue_t *priority_queue;
    struct resource_table_t *resource_table;
    struct process_t *active_process;
};

extern syscall_t syscall_table[32];

extern struct kernel_t kernel_state;

void construct_kernel_state(struct kernel_t *kernel, struct page_stack_t *page_stack, 
    struct priority_queue_t *priority_queue, struct resource_table_t *resource_table,
    size_t module_count, struct module_t *module_list);

size_t do_syscall(struct kernel_t *kernel, enum syscall_id_t id, size_t arg1, size_t arg2, size_t arg3);

int load_module(struct kernel_t *kernel, struct module_t *module);

struct process_context_t *next_process(struct kernel_t *kernel, struct process_context_t *prev_state);

int terminate_process(struct kernel_t *kernel, size_t process_id);

int accept_message(struct kernel_t *kernel, size_t process_id, struct message_t *message);

int send_message(struct kernel_t *kernel, size_t process_id, const struct message_t *message);

void panic(const char *message) __attribute__ ((noreturn));

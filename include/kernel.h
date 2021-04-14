#pragma once

#include "priorityqueue.h"
#include "module.h"
#include <stddef.h>

struct kernel_t
{
    struct priority_queue_t *priority_queue;
};

extern struct kernel_t kernel;

int do_syscall(struct kernel_t *kernel, size_t id, size_t arg1, size_t arg2, size_t arg3);

int load_module(struct kernel_t *kernel, struct module_t *module);

void panic(const char *message) __attribute__ ((noreturn));

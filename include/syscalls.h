#pragma once

#include "kernel.h"

size_t test_syscall(struct kernel_t *kernel, size_t arg1, size_t arg2, size_t arg3);

size_t mmap(struct kernel_t *kernel, size_t location, size_t length, size_t flags);

size_t munmap(struct kernel_t *kernel, size_t location, size_t length, size_t arg3);

size_t terminate_self(struct kernel_t *kernel, size_t arg1, size_t arg2, size_t arg3)

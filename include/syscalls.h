#pragma once

#include "types/syscallarg.h"
#include <stdbool.h>
#include <stddef.h>

typedef size_t (*syscall_ptr_0_t)();

typedef size_t (*syscall_ptr_1_t)(syscall_arg_t);

typedef size_t (*syscall_ptr_2_t)(syscall_arg_t, syscall_arg_t);

typedef size_t (*syscall_ptr_3_t)(syscall_arg_t, syscall_arg_t, syscall_arg_t);

struct syscall_t
{
    bool defined;
    int arg_count;
    int process_id;
    union
    {
        syscall_ptr_0_t func_ptr_0;
        syscall_ptr_1_t func_ptr_1;
        syscall_ptr_2_t func_ptr_2;
        syscall_ptr_3_t func_ptr_3;
    };
};

size_t test_syscall(syscall_arg_t str);

size_t mmap(syscall_arg_t location, syscall_arg_t length, syscall_arg_t flags);

size_t munmap(syscall_arg_t location, syscall_arg_t length);

size_t map_physical(syscall_arg_t arg_addr, syscall_arg_t arg_phys_addr, syscall_arg_t arg_length);

size_t unmap_physical(syscall_arg_t arg_addr, syscall_arg_t arg_length);

size_t terminate_self();

size_t send(syscall_arg_t recipient, syscall_arg_t message, syscall_arg_t flags);

size_t receive(syscall_arg_t buffer, syscall_arg_t flags);

size_t open_port(syscall_arg_t id);

size_t close_port(syscall_arg_t id);

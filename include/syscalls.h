#pragma once

#include <stdbool.h>
#include <stddef.h>

enum syscall_id_t
{
    SYSCALL_TEST = 1,
    SYSCALL_YIELD,
    SYSCALL_MMAP,
    SYSCALL_MUNMAP,
    SYSCALL_TERMINATE_SELF,
    SYSCALL_SEND,
    SYSCALL_RECEIVE,
    SYSCALL_OPEN_PORT,
    SYSCALL_CLOSE_PORT
};

typedef union
{
    long signed_int;
    unsigned long unsigned_int;
    void *ptr;
} syscall_arg_t;

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

size_t terminate_self();

size_t send(syscall_arg_t recipient, syscall_arg_t message, syscall_arg_t flags);

size_t receive(syscall_arg_t buffer, syscall_arg_t flags);

size_t openport(syscall_arg_t id);

size_t closeport(syscall_arg_t id);

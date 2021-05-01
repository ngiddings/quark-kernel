#pragma once

enum syscall_id_t
{
    SYSCALL_TEST = 1,
    SYSCALL_YIELD,
    SYSCALL_MMAP,
    SYSCALL_MUNMAP,
    SYSCALL_TERMINATE_SELF
};

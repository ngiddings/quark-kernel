#ifndef QUARK_SYSCALLID_H
#define QUARK_SYSCALLID_H

enum syscall_id_t
{
    SYSCALL_TEST = 1,
    SYSCALL_MMAP,
    SYSCALL_MUNMAP,
    SYSCALL_YIELD,
    SYSCALL_EXIT,
    SYSCALL_SEND,
    SYSCALL_RECEIVE,
    SYSCALL_OPEN_PORT,
    SYSCALL_CLOSE_PORT
};

#endif
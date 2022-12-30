#ifndef _QUARK_SYSCALLARG_H
#define _QUARK_SYSCALLARG_H

typedef union
{
    long int signed_int;
    unsigned long int unsigned_int;
    void *ptr;
} syscall_arg_t;

#endif
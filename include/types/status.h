#ifndef _QUARK_ERROR_H
#define _QUARK_ERROR_H

typedef enum
{
    ENONE = 0,
    ENULLPTR,
    EOUTOFBOUNDS,
    EINVALIDARG,
    ENOMEM,
    ENOTPRESENT,
    EDOESNTEXIST,
    EEXISTS,
    ENOSYSCALL,
    EPERM,
    EBUSY,
    EEXITED
} error_t;

#endif
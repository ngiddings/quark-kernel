#ifndef _QUARK_ERROR_H
#define _QUARK_ERROR_H

enum error_t
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
};

#endif
#pragma once

enum error_t
{
    ENONE = 0,
    ENULLPTR,
    EOUTOFBOUNDS,
    EINVALIDARG,
    ENOMEM,
    EDOESNTEXIST,
    EEXISTS,
    ENOSYSCALL,
    EPERM,
    EBUSY
};
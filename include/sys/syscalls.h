#ifndef _QUARK_SYSCALLS_H
#define _QUARK_SYSCALLS_S

#include <types/syscallarg.h>
#include <types/syscallid.h>
#include <types/physaddr.h>

unsigned long _do_syscall(syscall_id_t id, syscall_arg_t arg1, syscall_arg_t arg2, syscall_arg_t arg3);

static inline int kprint(const char *s)
{
    syscall_arg_t arg1;
    syscall_arg_t arg2;
    syscall_arg_t arg3;
    arg1.ptr = (void*)s;
    arg2.unsigned_int = 0UL;
    arg3.signed_int = 0UL;
    return (int) _do_syscall(SYSCALL_TEST, arg1, arg2, arg3);
}

static inline int mmap(void *addr, unsigned long length, long flags)
{
    syscall_arg_t arg1;
    syscall_arg_t arg2;
    syscall_arg_t arg3;
    arg1.ptr = addr;
    arg2.unsigned_int = length;
    arg3.signed_int = flags;
    return (int) _do_syscall(SYSCALL_MMAP, arg1, arg2, arg3);
}

static inline int munmap(void *addr, unsigned long length)
{
    syscall_arg_t arg1;
    syscall_arg_t arg2;
    syscall_arg_t arg3;
    arg1.ptr = addr;
    arg2.unsigned_int = length;
    arg3.unsigned_int = 0UL;
    return (int) _do_syscall(SYSCALL_MUNMAP, arg1, arg2, arg3);
}

static inline int map_physical(void *addr, physaddr_t phys_addr, unsigned long length)
{
    syscall_arg_t arg1;
    syscall_arg_t arg2;
    syscall_arg_t arg3;
    arg1.ptr = addr;
    arg2.unsigned_int = phys_addr;
    arg3.unsigned_int = length;
    return (int) _do_syscall(SYSCALL_MAP_PHYS, arg1, arg2, arg3);
}

static inline int unmap_physical(void *addr, unsigned long length)
{
    syscall_arg_t arg1;
    syscall_arg_t arg2;
    syscall_arg_t arg3;
    arg1.ptr = addr;
    arg2.unsigned_int = length;
    arg3.unsigned_int = 0UL;
    return (int) _do_syscall(SYSCALL_UNMAP_PHYS, arg1, arg2, arg3);
}

#endif
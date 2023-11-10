#ifndef _QUARK_SYSCALLS_H
#define _QUARK_SYSCALLS_H

#include <types/syscallarg.h>
#include <types/syscallid.h>
#include <types/physaddr.h>
#include <types/pid.h>
#include <types/sigaction.h>
#include <stddef.h>

/**
 * @brief Executes a system call
 * @param id The ID of the syscall to execute
 * @return An error code, with 0 indicating success
 */
int _do_syscall(syscall_id_t id, ...);

/**
 * @brief 
 * @param s The string to print 
 * @return 
 */
static inline int kprint(const char *s)
{
    return _do_syscall(SYSCALL_TEST, s);
}

/**
 * @brief 
 * @param addr 
 * @param length 
 * @param flags 
 * @return 
 */
static inline int map_anon(void *addr, unsigned long length, long flags)
{
    return _do_syscall(SYSCALL_MAP_ANON, addr, length, flags);
}

/**
 * @brief 
 * @param addr 
 * @param length 
 * @return 
 */
static inline int unmap_anon(void *addr, unsigned long length)
{
    return _do_syscall(SYSCALL_UNMAP_ANON, addr, length);
}

/**
 * @brief 
 * @param addr 
 * @param phys_addr 
 * @param length 
 * @return 
 */
static inline int map_physical(void *addr, physaddr_t phys_addr, unsigned long length)
{
    return _do_syscall(SYSCALL_MAP_PHYS, addr, phys_addr, length);
}

/**
 * @brief 
 * @param addr 
 * @param length 
 * @return 
 */
static inline int unmap_physical(void *addr, unsigned long length)
{
    return _do_syscall(SYSCALL_UNMAP_PHYS, addr, length);
}

/**
 * @brief 
 * @param id 
 * @return 
 */
static inline int open_port(int id)
{
    return _do_syscall(SYSCALL_OPEN_PORT, id);
}

/**
 * @brief 
 * @param id 
 * @return 
 */
static inline int close_port(int id)
{
    return _do_syscall(SYSCALL_CLOSE_PORT, id);
}

/**
 * @brief 
 * @param recipient 
 * @param message 
 * @param flags 
 * @return 
 */
static inline int send_pid(pid_t recipient, void *message, int flags)
{
    return _do_syscall(SYSCALL_SEND_PID, recipient, message, flags);
}

/**
 * @brief 
 * @param recipient 
 * @param message 
 * @param flags 
 * @return 
 */
static inline int send_port(int recipient, void *message, int flags)
{
    return _do_syscall(SYSCALL_SEND_PORT, recipient, message, flags);
}

/**
 * @brief 
 * @param buffer 
 * @param flags 
 * @return 
 */
static inline int receive(void *buffer, int flags)
{
    return _do_syscall(SYSCALL_RECEIVE, buffer, flags);
}

/**
 * @brief 
 * @param location 
 * @param size 
 * @param flags 
 * @return 
 */
static inline int create_object(void *location, size_t size, int flags)
{
    return _do_syscall(SYSCALL_CREATE_OBJECT, location, size, flags);
}

/**
 * @brief 
 * @param id 
 * @param location 
 * @return 
 */
static inline int aquire_object(int id, void *location)
{
    return _do_syscall(SYSCALL_AQUIRE_OBJECT, id, location);
}

/**
 * @brief 
 * @param id 
 * @return 
 */
static inline int release_object(int id)
{
    return _do_syscall(SYSCALL_RELEASE_OBJECT, id);
}

/**
 * @brief 
 * @return 
 */
static inline int get_pid()
{
    return _do_syscall(SYSCALL_GET_PID);
}

/**
 * @brief 
 * @param entry 
 * @param arg 
 * @param stack 
 * @param flags 
 * @return 
 */
static inline int clone(void (*entry)(void*), void *arg, void *stack, int flags)
{
    return _do_syscall(SYSCALL_CLONE, entry, arg, stack, flags);
}

/**
 * @brief 
 * @param id 
 * @param action 
 * @param flags 
 * @return 
 */
static inline int signal_action(int id, struct signal_action_t *action, int flags)
{
    return _do_syscall(SYSCALL_SIGNAL_ACTION, id, action, flags);
}

/**
 * @brief 
 * @return 
 */
static inline int signal_return()
{
    return _do_syscall(SYSCALL_SIGNAL_RETURN);
}

/**
 * @brief 
 * @param pid 
 * @param sigid 
 * @return 
 */
static inline int signal_raise(pid_t pid, int sigid)
{
    return _do_syscall(SYSCALL_SIGNAL_RAISE, pid, sigid);
}

/**
 * @brief 
 * @param id 
 * @param action 
 * @param flags 
 * @return 
 */
static inline int intr_action(int id, struct signal_action_t *action, int flags)
{
    return _do_syscall(SYSCALL_INTR_ACTION, id, action, flags);
}

/**
 * @brief 
 * @return 
 */
static inline int intr_return()
{
    return _do_syscall(SYSCALL_INTR_RETURN);
}

#endif
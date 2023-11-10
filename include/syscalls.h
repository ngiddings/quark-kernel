#ifndef QUARK_SYSCALL_DEF_H
#define QUARK_SYSCALL_DEF_H

#include "types/syscallarg.h"
#include "types/sigaction.h"
#include "types/pid.h"
#include "types/oid.h"
#include <stdbool.h>
#include <stddef.h>

extern void *syscall_table[];

typedef int (*syscall_ptr_0_t)();

typedef int (*syscall_ptr_1_t)(syscall_arg_t);

typedef int (*syscall_ptr_2_t)(syscall_arg_t, syscall_arg_t);

typedef int (*syscall_ptr_3_t)(syscall_arg_t, syscall_arg_t, syscall_arg_t);

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

/**
 * @brief Outputs the given string from kernel mode.
 * 
 * @param str The string to output.
 */
int test_syscall(syscall_arg_t str);

/**
 * 
 */
int syscall_map_anon(syscall_arg_t location, syscall_arg_t length, syscall_arg_t flags);

/**
 * 
 */
int syscall_unmap_anon(syscall_arg_t location, syscall_arg_t length);

/**
 * 
 */
int syscall_map_physical(syscall_arg_t arg_addr, syscall_arg_t arg_phys_addr, syscall_arg_t arg_length);

/**
 * 
 */
int syscall_unmap_physical(syscall_arg_t arg_addr, syscall_arg_t arg_length);

/**
 * 
 */
int syscall_open_port(syscall_arg_t id);

/**
 * 
 */
int syscall_close_port(syscall_arg_t id);

/**
 * 
 */
int syscall_send_pid(syscall_arg_t recipient, syscall_arg_t message, syscall_arg_t flags);

/**
 * 
 */
int syscall_send_port(syscall_arg_t recipient, syscall_arg_t message, syscall_arg_t flags);

/**
 * 
 */
int syscall_receive(syscall_arg_t buffer, syscall_arg_t flags);

/**
 * 
 */
int syscall_create_object(void *location, size_t size, int flags);

/**
 * 
 */
int syscall_aquire_object(oid_t id, void *location);

/**
 * 
 */
int syscall_release_object(oid_t id);

/**
 * 
 */
int syscall_get_pid();

/**
 * 
 */
int syscall_clone(void (*entry)(void*), void *arg, void *stack, int flags);

/**
 * 
 */
int syscall_signal_action(int id, struct signal_action_t *action, int flags);

/**
 * 
 */
int syscall_signal_return();

/**
 * 
 */
int syscall_signal_raise(pid_t pid, int sigid);

/**
 * 
 */
int syscall_intr_action(int id, struct signal_action_t *action, int flags);

/**
 * 
 */
int syscall_intr_return();

#endif

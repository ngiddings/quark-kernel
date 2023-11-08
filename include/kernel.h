#pragma once

#include "avltree.h"
#include "priorityqueue.h"
#include "queue.h"
#include "mmgr.h"
#include "syscalls.h"
#include "types/syscallid.h"
#include "types/status.h"
#include "types/pid.h"
#include "types/oid.h"
#include "types/sighandler.h"
#include <libmalloc/memmap.h>
#include <stddef.h>

#define MAX_SYSCALL_ID 256
#define MODULE_LIMIT 8

#define IO_OP 1 << 0
#define IO_SYNC 0 << 0
#define IO_ASYNC 1 << 0

#define IO_RECIPIENT_TYPE 3 << 1
#define IO_PID 0 << 1
#define IO_PORT 1 << 1
#define IO_MAILBOX 2 << 1

struct process_context_t;

struct module_t
{
    physaddr_t start;
    physaddr_t end;
    char str[64 - 2 * sizeof(physaddr_t)];
};

struct boot_info_t
{
    char *bootloader;
    char *parameters;
    size_t memory_size;
    memory_map_t map;
    size_t module_count;
    struct module_t modules[MODULE_LIMIT];
};

struct message_t
{
    pid_t sender;
    unsigned long code;
    unsigned long args[6];
};

void kernel_initialize(struct boot_info_t *boot_info);

error_t kernel_set_syscall(int id, int arg_count, void *func_ptr);

size_t kernel_do_syscall(syscall_id_t id, syscall_arg_t arg1, syscall_arg_t arg2, syscall_arg_t arg3, void *pc, void *stack, unsigned long flags);

error_t kernel_load_module(struct module_t *module);

pid_t kernel_current_pid();

struct process_context_t *kernel_current_context();

error_t kernel_store_active_context(struct process_context_t *context);

pid_t kernel_spawn_process(void *program_entry, int priority, physaddr_t address_space);

struct process_context_t *kernel_advance_scheduler();

error_t kernel_terminate_process(pid_t process_id);

error_t kernel_create_port(unsigned long id);

error_t kernel_remove_port(unsigned long id);

pid_t kernel_get_port_owner(unsigned long id);

error_t kernel_send_message(unsigned long recipient, struct message_t *message);

error_t kernel_queue_message(unsigned long recipient, struct message_t *message);

int kernel_receive_message(struct message_t *buffer, int flags);

error_t kernel_register_interrupt_handler(unsigned long interrupt, signal_handler_t handler, void *userdata);

error_t kernel_remove_interrupt_handler(unsigned long interrupt);

error_t kernel_execute_interrupt_handler(unsigned long interrupt);

error_t kernel_signal_return();

error_t kernel_create_object(size_t size, unsigned long flags, oid_t *id);

error_t kernel_attach_object(oid_t id, void *virt_addr);

error_t kernel_release_object(oid_t id);

void kernel_panic(const char *message) __attribute__ ((noreturn));

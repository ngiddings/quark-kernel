#pragma once

#include "avltree.h"
#include "priorityqueue.h"
#include "queue.h"
#include "mmgr.h"
#include "syscalls.h"
#include "types/syscallid.h"
#include "types/status.h"
#include <libmalloc/memmap.h>
#include <stddef.h>

#define MAX_SYSCALL_ID 256
#define module_limit 8

#define IO_OP 1 << 0
#define IO_SYNC 0 << 0
#define IO_ASYNC 1 << 0

#define IO_RECIPIENT_TYPE 3 << 1
#define IO_PID 0 << 1
#define IO_PORT 1 << 1
#define IO_MAILBOX 2 << 1

typedef unsigned long (*signal_handler_t)(void*, void*);

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
    struct module_t modules[module_limit];
};

struct message_t
{
    unsigned long sender;
    unsigned long code;
    unsigned long args[6];
};

enum process_state_t
{
    PROCESS_ACTIVE,
    PROCESS_REQUESTING,
    PROCESS_SENDING
};

struct process_t
{
    unsigned long pid;
    int priority;
    physaddr_t page_table;
    enum process_state_t state;
    struct queue_t sending_queue;
    struct queue_t message_queue;
    struct message_t *message_buffer;
    struct process_context_t *ctx;
};

struct port_t
{
    unsigned long id;
    unsigned long owner_pid;
};

struct signal_action_t
{
    unsigned long pid;
    signal_handler_t func_ptr;
    void (*trampoline_ptr)();
    void *userdata;
};

struct signal_context_t
{
    unsigned long signal_id;
};

struct kernel_t
{
    struct syscall_t syscall_table[MAX_SYSCALL_ID];
    struct priority_queue_t priority_queue;
    struct avltree_t *interrupt_handlers;
    struct avltree_t *port_table;
    struct avltree_t *process_table;
    struct process_t *active_process;
    int next_pid;
};

void kernel_initialize(struct boot_info_t *boot_info);

error_t set_syscall(int id, int arg_count, int pid, void *func_ptr);

size_t do_syscall(syscall_id_t id, syscall_arg_t arg1, syscall_arg_t arg2, syscall_arg_t arg3, void *pc, void *stack, unsigned long flags);

error_t kernel_load_module(struct module_t *module);

unsigned long kernel_current_pid();

struct process_context_t *kernel_current_context();

error_t kernel_store_active_context(struct process_context_t *context);

unsigned long kernel_spawn_process(void *program_entry, int priority, physaddr_t address_space);

struct process_context_t *kernel_advance_scheduler();

error_t kernel_terminate_process(size_t process_id);

error_t kernel_create_port(unsigned long id);

error_t kernel_remove_port(unsigned long id);

unsigned long kernel_get_port_owner(unsigned long id);

error_t kernel_send_message(unsigned long recipient, struct message_t *message);

error_t kernel_queue_message(unsigned long recipient, struct message_t *message);

error_t kernel_register_interrupt_handler(unsigned long interrupt, signal_handler_t handler, void *userdata);

error_t kernel_remove_interrupt_handler(unsigned long interrupt);

error_t kernel_execute_interrupt_handler(unsigned long interrupt);

error_t kernel_signal_return();

int receive_message(struct message_t *buffer, int flags);

void panic(const char *message) __attribute__ ((noreturn));

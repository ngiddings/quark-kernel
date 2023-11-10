#ifndef QUARK_PROCESS_H
#define QUARK_PROCESS_H

#include "types/pid.h"
#include "types/physaddr.h"
#include "types/status.h"
#include "queue.h"
#include "addressspace.h"

typedef enum process_state_t
{
    PROCESS_ACTIVE,
    PROCESS_REQUESTING,
    PROCESS_SENDING
} process_state_t;

typedef struct process_t
{
    pid_t pid;
    int priority;
    address_space_t *address_space;
    struct avltree_t *shared_objects;
    process_state_t state;
    struct queue_t sending_queue;
    struct queue_t message_queue;
    struct message_t *message_buffer;
    struct process_context_t *ctx;
} process_t;

process_t *process_construct(pid_t pid, void *entry, void *stack, int priority, address_space_t *address_space);

#endif
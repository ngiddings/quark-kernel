#include "process.h"
#include "heap.h"
#include "mmgr.h"
#include "platform/context.h"
#include "string.h"
#include "avltree.h"

process_t *process_construct(pid_t pid, void *entry, void *stack, int priority, address_space_t *address_space)
{
    process_t *new_process = (process_t*) kmalloc(sizeof(process_t));
    if(new_process == NULL) 
    {
        return NULL;
    }

    struct process_context_t *initial_context = kmalloc(sizeof(struct process_context_t));
    if(initial_context == NULL)
    {
        return NULL;
    }

    if(address_space == NULL 
        && (address_space = address_space_construct()) == NULL)
    {
        return NULL;
    }

    context_construct(initial_context);
    set_context_pc(initial_context, entry);
    set_context_stack(initial_context, stack);
    new_process->priority = priority;
    new_process->pid = pid;
    new_process->address_space = address_space;
    new_process->address_space->refcount++;
    new_process->shared_objects = NULL;
    new_process->state = PROCESS_ACTIVE;
    new_process->message_buffer = NULL;
    new_process->ctx = initial_context;
    queue_construct(&new_process->message_queue);
    return new_process;
}

error_t process_queue_message(process_t *process, message_t *msg)
{
    return queue_insert(&process->message_queue, msg);
}

error_t process_receive_message(process_t *process, message_t *buffer)
{
    if(process->message_queue.count == 0)
    {
        return EDOESNTEXIST;
    }

    message_t *queued_message = queue_get_next(&process->message_queue);
    memcpy(buffer, queued_message, sizeof(message_t));
    kfree(queued_message);
    return ENONE;
}

error_t process_store_object(process_t *process, int id, void *location)
{
    process->shared_objects = avl_insert(process->shared_objects, id, location);
    return ENONE;
}

void *process_get_object(process_t *process, int id)
{
    return avl_get(process->shared_objects, id);
}
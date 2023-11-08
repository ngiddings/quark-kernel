#include "process.h"
#include "heap.h"
#include "platform/context.h"
#include "string.h"

process_t *process_construct(pid_t pid, void *entry, void *stack, int priority, physaddr_t address_space)
{
    process_t *new_process = (process_t*) kmalloc(sizeof(process_t));
    if(new_process == NULL) 
    {
        return NULL;
    }

    struct process_context_t *initial_context = kmalloc(sizeof(struct process_context_t));
    if(initial_context == NULL)
    {
        return 0;
    }

    context_construct(initial_context);
    set_context_pc(initial_context, entry);
    set_context_stack(initial_context, stack);
    new_process->priority = priority;
    new_process->pid = pid;
    new_process->page_table = address_space;
    new_process->state = PROCESS_ACTIVE;
    new_process->message_buffer = NULL;
    new_process->ctx = initial_context;
    queue_construct(&new_process->sending_queue);
    queue_construct(&new_process->message_queue);
    return new_process;
}
#include "platform/context.h"
#include "kernel.h"
#include "mmgr.h"
#include "heap.h"
#include "string.h"
#include "system.h"
#include <stdarg.h>

void context_construct(struct process_context_t *context)
{
    memset(context, 0, sizeof(struct process_context_t));
    context->flags = DEFAULT_FLAGS;
}

void *get_context_pc(struct process_context_t *context)
{
    return (void*) context->eip;
}

void set_context_pc(struct process_context_t *context, void *pc)
{
    context->eip = (unsigned long) pc;
    context->cs = 0x1B;
}

void *get_context_stack(struct process_context_t *context)
{
    return (void*) context->gp_registers[7];
}

void set_context_stack(struct process_context_t *context, void *stack)
{
    context->gp_registers[7] = (unsigned long)stack;
    context->ss = 0x23;
}

void set_context_flags(struct process_context_t *context, unsigned long flags)
{
    context->flags = flags;
}

void set_context_return(struct process_context_t *context, unsigned long value)
{
    context->gp_registers[0] = value;
}

void *context_stack_push(struct process_context_t *context, unsigned long value)
{
    context->gp_registers[7] -= sizeof(long);
    *((unsigned long*) context->gp_registers[7]) = value;
    return (void*) context->gp_registers[7];
}

void *context_stack_push_struct(struct process_context_t *context, void *data, unsigned long size)
{
    context->gp_registers[7] -= size;
    memcpy((void*) context->gp_registers[7], data, size);
    return (void*) context->gp_registers[7];
}

void *context_stack_pop(struct process_context_t *context, void *value)
{
    unsigned long *prev_stack = ((unsigned long*) context->gp_registers[7]);
    context->gp_registers[7] += sizeof(unsigned long);
    *((unsigned long*) value) = *prev_stack;
    return (void*) context->gp_registers[7];
}

void *context_stack_pop_struct(struct process_context_t *context, void *value, unsigned long size)
{
    void *prev_stack = ((void*) context->gp_registers[7]);
    context->gp_registers[7] += size;
    memcpy(value, prev_stack, size);
    return (void*) context->gp_registers[7];
}

void context_call_func(struct process_context_t *context, void *func_ptr, void *ret_ptr, int argc, ...)
{
    context->eip = (uint32_t) func_ptr;
    va_list valist;
    va_start(valist, argc);
    for(int i = 0; i < argc; i++)
    {
        context_stack_push(context, va_arg(valist, unsigned long));
    }
    context_stack_push(context, (unsigned long) ret_ptr);
}

void context_cleanup_func(struct process_context_t *context, int argc)
{
    context->gp_registers[7] -= sizeof(unsigned long) * argc;
}
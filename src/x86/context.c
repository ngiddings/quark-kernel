#include "platform/context.h"
#include "kernel.h"
#include "mmgr.h"
#include "heap.h"
#include "string.h"
#include "system.h"

void set_context_pc(struct process_context_t *context, void *pc)
{
    context->eip = pc;
    context->cs = 0x1B;
}

void set_context_stack(struct process_context_t *context, void *stack)
{
    context->gp_registers[7] = stack;
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
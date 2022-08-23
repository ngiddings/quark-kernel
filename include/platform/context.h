#ifndef _QUARK_CONTEXT_H
#define _QUARK_CONTEXT_H

#include <stdint.h>

#if defined __i386__ || defined __x86_64__
#define DEFAULT_FLAGS 0x3200
#endif

/**
 * @brief Stores the context of a particular process. The contents of this
 * struct are platform-specific.
 * 
 */
struct process_context_t
{

#if defined __i386__
    uint32_t gp_registers[8];
    uint32_t ss;
    uint32_t cs;
    uint32_t eip;
    uint32_t flags;
#elif defined __x86_64__
    uint64_t gp_registers[16];
    uint64_t ss;
    uint64_t cs;
    uint64_t rip;
    uint64_t flags;
#endif

};

void load_context(struct process_context_t *context);

void set_context_pc(struct process_context_t *context, void *pc);

void set_context_stack(struct process_context_t *context, void *stack);

void set_context_flags(struct process_context_t *context, unsigned long flags);

void set_context_return(struct process_context_t *context, unsigned long value);

#endif

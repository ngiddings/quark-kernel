#include "platform/context.h"
#include "kernel.h"
#include "mmgr.h"
#include "heap.h"
#include "string.h"
#include "system.h"
#include "x86/processstate.h"

void *initialize_context(void *task_entry)
{
    physaddr_t stack_frame = reserve_page();
    if(stack_frame % page_size != 0)
    {
        return NULL;
    }
    map_page((void*)&_kernel_start - page_size, stack_frame, PAGE_RW | PAGE_USERMODE);
    unmap_page((void*)&_kernel_start - (2 * page_size));
    struct process_context_t *context = kmalloc(sizeof(struct process_context_t));
    if(context != NULL)
    {
       memset(context, 0, sizeof(struct process_context_t));
       uint32_t flags;
       asm("pushf; "
           "mov (%%esp), %0; "
           "popf; "
           : "=r"(flags));
       context->cs = 0x1B;
       context->eip = (uint32_t)task_entry;
       context->flags = (flags & ~0xFD) | 0x200;
       context->ss = 0x23;
       context->esp = &_kernel_start;
       context->ebp = &_kernel_start;
    }
    return (void*)context;
}

void destroy_context(void *ctx)
{
    kfree(ctx);
}

void save_context(struct process_context_t *context)
{
    store_active_context(context, sizeof(*context));
}
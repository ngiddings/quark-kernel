#include "platform/context.h"
#include "mmgr.h"
#include "string.h"
#include "x86/processstate.h"

void *initialize_context(void *task_entry)
{
    /* 
     * TODO: this implementation is a goddamn mess.
     * Stack pointer is hardcoded, and the stack isn't resizable.
     * PCB pointer is just a constant.
     */
    map_page(NULL, reserve_page(), PAGE_RW);
    map_page((void*)0xFF3FF000, reserve_page(), PAGE_RW | PAGE_USERMODE);
    unmap_page((void*)0xFF3FE000);
    uint32_t flags;
    asm("pushf; "
        "mov (%%esp), %0; "
        "popf; "
        : "=r"(flags));
    struct process_context_t *state = (struct process_context_t*)PCB_LOCATION;
    memset(NULL, 0, page_size);
    state->cs = 0x1B;
    state->eip = (uint32_t)task_entry;
    state->flags = (flags & ~0xFD) | 0x200;
    state->ss = 0x23;
    state->esp = 0xFF400000;
    state->ebp = 0xFF400000;
    return (void*)state;
}

void destroy_context(void *ctx)
{
    // Nothing to do...
}
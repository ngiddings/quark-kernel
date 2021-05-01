#include "context.h"
#include "pageallocator.h"
#include "mmgr.h"
#include "string.h"
#include "x86/processstate.h"

void *initialize_context(void *task_entry, struct page_stack_t *page_stack)
{
    map_page(page_stack, (void*)NULL, reserve_page(page_stack), PAGE_RW);
    map_page(page_stack, (void*)0xFF7FF000, reserve_page(page_stack), PAGE_RW | PAGE_USERMODE);
    unmap_page((void*)0xFF7FE000);
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
    state->esp = 0xFF800000;
    state->ebp = 0xFF800000;
    return (void*)state;
}

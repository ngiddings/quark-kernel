#include "context.h"
#include "pageallocator.h"
#include "mmgr.h"

void *initialize_context(void *task_entry, struct page_stack_t *page_stack)
{
    physaddr_t stack0 = reserve_page(page_stack);
    map_page(page_stack, (void*)0xFF7FF000, stack0, PAGE_RW);
    map_page(page_stack, (void*)0xFF7FD000, stack0, PAGE_RW | PAGE_USERMODE);
    unmap_page((void*)0xFF7FE000);
    unmap_page((void*)0xFF7FC000);
    uint32_t flags;
    uint32_t *stack = (uint32_t*)((void*)stack0 - 20);
    asm("pushf; "
        "mov (%%esp), %0; "
        "popf; "
        : "=r"(flags));
    stack[0] = (uint32_t)task_entry;
    stack[1] = 27;
    stack[2] = flags;
    stack[3] = 0xFF7FE000;
    stack[4] = 35;
    return (void*)stack;
}

void load_context(struct process_state_t *context)
{
    asm("mov %0, %%esp; "
        "popal; "
        "iret; "
        :: "r"(context));
}
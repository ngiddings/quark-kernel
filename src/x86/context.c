#include "context.h"
#include "pageallocator.h"
#include "mmgr.h"
#include "string.h"

struct process_state_t
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_temp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t eip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

void *initialize_context(void *task_entry, struct page_stack_t *page_stack)
{
    map_page(page_stack, (void*)0xFF7FF000, reserve_page(page_stack), PAGE_RW);
    map_page(page_stack, (void*)0xFF7FD000, reserve_page(page_stack), PAGE_RW | PAGE_USERMODE);
    unmap_page((void*)0xFF7FE000);
    unmap_page((void*)0xFF7FC000);
    uint32_t flags;
    struct process_state_t *stack = (struct process_state_t*)((void*)0xFF800000 - 20 - 8*4);
    asm("pushf; "
        "mov (%%esp), %0; "
        "popf; "
        : "=r"(flags));
    memset(stack, 0, sizeof(*stack));
    stack->eip = (uint32_t)task_entry;
    stack->cs = 27;
    stack->flags = flags;
    stack->esp = 0xFF7FE000;
    stack->ss = 35;
    stack->esp_temp = &stack->eax;
    return (void*)stack;
}

void load_context(struct process_state_t *context)
{
    asm("mov %0, %%esp; "
        "popal; "
        "iret; "
        :: "r"(context));
}
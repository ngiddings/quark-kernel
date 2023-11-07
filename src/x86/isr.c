#include "x86/isr.h"
#include "stdio.h"
#include "x86/apic.h"
#include "platform/interrupts.h"
#include "mmgr.h"
#include "string.h"
#include "kernel.h"

#include <stdint.h>

struct interrupt_frame_t
{
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
};

typedef struct page_fault_code_t
{
    uint32_t present : 1;
    uint32_t write : 1;
    uint32_t usermode : 1;
    uint32_t reserved : 1;
    uint32_t inst_fetch : 1;
    uint32_t pk : 1;
    uint32_t shadow_stack : 1;
    uint32_t padding : 25;
} page_fault_code_t;

void isr_generic(struct interrupt_frame_t *frame)
{
    printf("Generic interrupt.\n");
}

void isr_division_by_zero(struct interrupt_frame_t *frame)
{
    printf("Exception: Division by zero\n");
}

void isr_segment_not_present(struct interrupt_frame_t *frame, unsigned int error)
{
    irq_disable();
    printf("Exception: NP fault, code %08x\n", error);
    asm("hlt");
}

void isr_gp_fault(struct interrupt_frame_t *frame, unsigned int error)
{
    irq_disable();
    asm("mov $0x10, %%ax; "
        "mov %%ax, %%ds; "
        "mov %%ax, %%es; "
        "mov %%ax, %%fs; "
        "mov %%ax, %%gs; " ::
            : "ax");
    printf("Exception: GP fault, code %08x\n", error);
    asm("hlt");
}

void isr_page_fault(struct interrupt_frame_t *frame, unsigned int error)
{
    page_fault_code_t *code = &error;
    uint32_t addr;
    asm("mov %%cr2, %0"
        : "=r"(addr));
    asm("mov $0x10, %%ax; "
        "mov %%ax, %%ds; "
        "mov %%ax, %%es; "
        "mov %%ax, %%fs; "
        "mov %%ax, %%gs; " ::
            : "ax");
    if(code->usermode == 0
        && code->present == 0
        && addr >= 0xFFC00000
        && addr < 0xFFFFF000)
    {
        printf("Allocating new page table %08x within fault handler.\n", addr);
        physaddr_t new_table = reserve_page();
        if(new_table == ENOMEM)
        {
            kernel_panic("Out of memory while allocating page table.\n");
        }
        set_pte((void*)addr, 1, PAGE_PRESENT | PAGE_USERMODE | PAGE_RW, new_table);
        asm volatile("mov %%cr3, %%eax;"
                     "mov %%eax, %%cr3" ::
                        : "eax", "memory");
        memset((void*)(addr & ~0xFFF), 0, page_size);
    }
    else
    {
        printf("Exception: Page fault, code %08x, linear address %08x\n", error, addr);
        kernel_panic("Unhandled page fault.\n");
    }
}

void isr_double_fault(struct interrupt_frame_t *frame, unsigned int error)
{
    irq_disable();
    printf("Exception: Double fault (!!), code %08x\n", error);
    asm("hlt");
}

void isr_timer(struct interrupt_frame_t *frame)
{
    printf("Timer tick.\n");
    apic_eoi();
}

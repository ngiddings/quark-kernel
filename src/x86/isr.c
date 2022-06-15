#include "x86/isr.h"
#include "stdio.h"
#include "x86/apic.h"
#include "platform/interrupts.h"

#include <stdint.h>

struct interrupt_frame_t
{
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
};

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
    uint32_t addr;
    asm("mov %%cr2, %0"
        : "=r"(addr));
    asm("mov $0x10, %%ax; "
        "mov %%ax, %%ds; "
        "mov %%ax, %%es; "
        "mov %%ax, %%fs; "
        "mov %%ax, %%gs; " ::
            : "ax");
    printf("Exception: Page fault, code %08x, linear address %08x\n", error, addr);
    asm("hlt");
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

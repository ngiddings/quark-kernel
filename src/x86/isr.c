#include "kernel.h"
#include "isr.h"
#include "stdio.h"
#include "apic.h"

void isr_generic(void* frame)
{
    printf("Generic interrupt.\n");
}

void isr_division_by_zero(void* frame)
{
    printf("Exception: Division by zero\n");
}

void isr_gp_fault(void* frame, unsigned int error)
{
    asm("cli");
    asm("mov $0x10, %%ax; "
        "mov %%ax, %%ds; "
        "mov %%ax, %%es; "
        "mov %%ax, %%fs; "
        "mov %%ax, %%gs; "
        ::: "ax");
    printf("Exception: GP fault, code %08x\n", error);
    asm("hlt");
}

void isr_page_fault(void* frame, unsigned int error)
{
    size_t addr;
    asm("mov %%cr2, %0"
        : "=r"(addr));
    asm("mov $0x10, %%ax; "
        "mov %%ax, %%ds; "
        "mov %%ax, %%es; "
        "mov %%ax, %%fs; "
        "mov %%ax, %%gs; "
        ::: "ax");
    printf("Exception: Page fault, code %08x, linear address %08x\n", error, addr);
    asm("hlt");
}

void isr_double_fault(void* frame, unsigned int error)
{
    asm("cli");
    
    printf("Exception: Double fault (!!), code %08x\n", error);
    asm("hlt");
}

void isr_timer(void* frame)
{
    printf("Timer tick.\n");
    apic_eoi();
}

void isr_preempt(void* frame)
{
    asm("pushal; "
        "mov %esp, %ebp; ");
    asm("mov $0x10, %%ax; "
        "mov %%ax, %%ds; "
        "mov %%ax, %%es; "
        "mov %%ax, %%fs; "
        "mov %%ax, %%gs; "
        ::: "ax");
    struct process_state_t *process_state;
    asm("mov %%ebp, %0"
        : "=r"(process_state));
    printf("Preempted process %08x.\n", kernel_state.active_process);
    apic_eoi();
    next_process(&kernel_state, process_state);
}

void isr_ap_start(void* frame)
{
    asm(".code16");
    //...
    asm(".code32");
    // do something useful
}

void isr_syscall(void* frame)
{
    
}
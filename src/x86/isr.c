#include "isr.h"
#include "stdio.h"
#include "apic.h"

__attribute__ ((interrupt))
void isr_generic(void* frame)
{
    printf("Generic interrupt.\n");
}

__attribute__ ((interrupt))
void isr_division_by_zero(void* frame)
{
    printf("Exception: Division by zero\n");
}

__attribute__ ((interrupt))
void isr_gp_fault(void* frame, unsigned int error)
{
    asm("cli");
    printf("Exception: GP fault, code %08x\n", error);
    asm("hlt");
}

__attribute__ ((interrupt))
void isr_page_fault(void* frame, unsigned int error)
{
    printf("Exception: Page fault\n");
}

__attribute__ ((interrupt))
void isr_double_fault(void* frame, unsigned int error)
{
    asm("cli");
    printf("Exception: Double fault (!!), code %08x\n", error);
    asm("hlt");
}

__attribute__ ((interrupt))
void isr_timer(void* frame)
{
    printf("Timer tick.\n");
    apic_eoi();
}

__attribute__ ((naked))
void isr_ap_start(void* frame)
{
    asm(".code16");
    //...
    asm(".code32");
    // do something useful
}

__attribute__ ((interrupt))
void isr_syscall(void* frame)
{
    
}
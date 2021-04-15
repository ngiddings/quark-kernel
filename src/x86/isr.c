#include "isr.h"
#include "stdio.h"

__attribute__ ((interrupt))
void isr_division_by_zero(void* frame)
{
    printf("Exception: Division by zero\n");
}

__attribute__ ((interrupt))
void isr_gp_fault(void* frame, unsigned int error)
{
    
}

__attribute__ ((interrupt))
void isr_page_fault(void* frame, unsigned int error)
{
    
}

__attribute__ ((interrupt))
void isr_double_fault(void* frame, unsigned int error)
{
    asm("hlt");
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
#include "platform/interrupts.h"
#include "x86/apic.h"

int initialize_interrupts()
{
    apic_enable();
    return 0;
}

void irq_enable()
{
    asm("sti");
}

void irq_disable()
{
    asm("cli");
}
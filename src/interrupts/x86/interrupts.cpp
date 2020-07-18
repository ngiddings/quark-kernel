#include "../interrupts.hpp"
#include "idt.hpp"
#include "inthandlers.hpp"

kernel::Interrupts::Interrupts()
{
    idt[0] = InterruptDescriptor((void*) &divisionByZero, InterruptDescriptor::INT32, 0);
    idt[8] = InterruptDescriptor((void*) &doubleFaultHandler, InterruptDescriptor::INT32, 0);
    idt[0x80] = InterruptDescriptor((void*) &syscallHandler, InterruptDescriptor::INT32, 0);
    // Load interrupt handlers
    // Configure PIC
    _lidt();
}

inline void kernel::Interrupts::enable()
{
    asm("sti");
}

inline void kernel::Interrupts::disable()
{
    asm("cli");
}
#include "../interrupts.hpp"
#include "idt.hpp"

kernel::Interrupts::Interrupts()
{
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
#ifndef IDT_H
#define IDT_H

#include "interruptdescriptor.hpp"

extern kernel::InterruptDescriptor idt[256];

extern "C" void _lidt();

#endif
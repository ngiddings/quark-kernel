noinst_LIBRARIES = libinterrupts.a
libinterrupts_a_CPPFLAGS = -ffreestanding -mgeneral-regs-only -O0 -Wall -fno-exceptions -fno-rtti -ggdb

if x86
libinterrupts_a_SOURCES = x86/interrupts.cpp x86/inthandlers.cpp x86/interruptdescriptor.cpp x86/idt.S
endif
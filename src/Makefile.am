noinst_PROGRAMS = quark-kernel
quark_kernel_SOURCES = quarkkernel.cpp elf.cpp tty.cpp systeminfo.cpp util.cpp memorymap.cpp buddyallocator.cpp 
quark_kernel_LDADD = -lgcc
quark_kernel_CPPFLAGS = -ffreestanding -mgeneral-regs-only -O0 -Wall -fno-exceptions -fno-rtti -ggdb
quark_kernel_LDFLAGS = -nostdlib

if x86
quark_kernel_SOURCES += x86/pagetableentry.cpp \
                        x86/mmap.cpp \
                        x86/interrupts.cpp \
                        x86/inthandlers.cpp \
                        x86/interruptdescriptor.cpp \
                        x86/idt.S \
                        x86/entry.S \
                        x86/pio.S \
                        x86/multiboot2header.S
quark_kernel_LDFLAGS += -T x86/linker.ld
quark_kernel_DEPENDENCIES = x86/linker.ld
endif

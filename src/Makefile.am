noinst_PROGRAMS = quark-kernel
quark_kernel_SOURCES = startup.cpp elf.cpp tty.cpp systeminfo.cpp util.cpp memorymap.cpp pageallocator.cpp 
quark_kernel_LDADD = -lgcc
quark_kernel_CPPFLAGS = -ffreestanding -mgeneral-regs-only -O0 -Wall -fno-exceptions -fno-rtti -ggdb
quark_kernel_LDFLAGS = -nostdlib

if x86
quark_kernel_SOURCES += x86/mmap.cpp \
                        x86/interrupts.cpp \
                        x86/entry.S 
quark_kernel_LDFLAGS += -T x86/linker.ld
quark_kernel_DEPENDENCIES = x86/linker.ld
endif

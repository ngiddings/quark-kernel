noinst_PROGRAMS = quark-kernel
quark_kernel_SOURCES = kernel.c memorymap.c pageallocator.c priorityqueue.c stdio.c string.c
quark_kernel_LDADD = -lgcc
quark_kernel_CFLAGS = -I$(top_srcdir)/include -ffreestanding -mgeneral-regs-only -O0 -Wall -ggdb
quark_kernel_LDFLAGS = -nostdlib

if x86
quark_kernel_SOURCES += x86/mmgr.c \
                        x86/putc.c \
                        x86/multiboot2.c \
                        x86/interrupts.c \
                        x86/apic.c \
                        x86/isr.c \
                        x86/quark_x86.c \
                        x86/entry.S 
quark_kernel_LDFLAGS += -T x86/linker.ld
quark_kernel_DEPENDENCIES = x86/linker.ld
endif


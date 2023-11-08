bin_PROGRAMS = quark-kernel
quark_kernel_SOURCES = kernel.c mmgr.c priorityqueue.c stdio.c string.c elf.c syscalls.c heap.c avltree.c queue.c math.c process.c sharedobject.c
quark_kernel_LDADD = -lgcc -lmalloc
quark_kernel_CFLAGS = -I$(top_srcdir)/include -I$(prefix)/include -ffreestanding -mgeneral-regs-only -O0 -Wall -ggdb
quark_kernel_LDFLAGS = -L$(prefix)/lib -nostdlib

lib_LIBRARIES = libquark-syscall.a
libquark_syscall_a_CFLAGS = -I$(top_srcdir)/include -I$(prefix)/include -ffreestanding -mgeneral-regs-only -O0 -Wall -ggdb

if x86
quark_kernel_SOURCES += x86/paging.c \
                        x86/putc.c \
                        x86/multiboot2.c \
                        x86/idt.c \
                        x86/interrupts.c \
                        x86/apic.c \
                        x86/isr.c \
                        x86/msr.c \
                        x86/context.c \
                        x86/load_context.S \
                        x86/preempt.S \
                        x86/quark_x86.c \
                        x86/entry.S
libquark_syscall_a_SOURCES = x86/do_syscall.S
quark_kernel_LDFLAGS += -T x86/linker.ld
quark_kernel_DEPENDENCIES = x86/linker.ld
endif


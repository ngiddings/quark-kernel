SUBDIRS = interrupts mmgr
noinst_PROGRAMS = quark-kernel
quark_kernel_SOURCES = quarkkernel.cpp tty.cpp systeminfo.cpp util.cpp entry.S pio.S multiboot2header.S
quark_kernel_LDADD = -lgcc mmgr/libmmgr.a interrupts/libinterrupts.a
quark_kernel_CPPFLAGS = -ffreestanding -O0 -Wall -fno-exceptions -fno-rtti -ggdb
quark_kernel_LDFLAGS = -T linker.ld -nostdlib
quark_kernel_DEPENDENCIES = linker.ld


# Quark Kernel

Quark Kernel is a hobbyist OS kernel mainly indended to be a challenging side-project.

## Building

You will need:
- A cross-compiler
- GNU Autotools

To build the kernel for the x86 platform, run:
- `autoreconf -i`
- `./configure --host=i686-elf --prefix=<your_desired_prefix> --bindir=$(prefix)/apps CFLAGS=-ffreestanding LDFLAGS=-nostdlib`
- `make`

To generate a bootable disk image, run:
- `make quark.iso`

So far this code has only been tested to compile using GCC. Some modifications might be necessary to build this project using other compilers. For a guide on building
a cross-compiler, see the article on [osdev.org](https://wiki.osdev.org/GCC_Cross-Compiler).
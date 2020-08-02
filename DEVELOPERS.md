Notice: The kernel is still in early development. Some information in this file
may be inaccurate. Please create an issue if this is the case.

# Porting

The kernel should be portable to most systems, provided they support paging
and have sufficient memory. All platform-specific code belongs in an appropriately-named
subdirectory inside src/. It must provide a linker script, startup code, and
implement a set of functions required by the kernel.

The linker script must define the following symbols:
- _pageMapLocation      The location of the bitmap to be used by the page allocator.
- _heapLocation         The location of the kernel's heap.
- _heapSize             The size in bytes of the kernel's heap.
- _kernelStart          The linear address of the start of the kernel image.
- _kernelEnd            The first linear address after the end of the kernel image.

The startup code is required to enable paging and relocate the kernel if necessary. It also
needs to provide a memory map to the kernel in the form of an array of MemoryMap::Region
structures. It should then call main() with a pointer to the kernel command line as an argument.

In addition, the contents of mmap.hpp and interrupts.hpp must be implemented.

# Files

[TODO]
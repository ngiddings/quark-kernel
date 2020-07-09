#include <stddef.h>
#include <stdint.h>

#include "systypes.hpp"
#include "systeminfo.hpp"
#include "memorymap.hpp"
#include "buddyallocator.hpp"
#include "addressspace.hpp"
#include "tty.h"

#if __STDC_HOSTED__ == 1 || __i686__ != 1
#error "ERROR: This program must be compiled for a freestanding environment, and currently only supports the i686 target."
#endif

using namespace qkernel;

extern SystemInfo system_info;
extern MemoryMap::Region memory_map;

void main(char* cmdline)
{
        TTY tty((char*) 0xC00B8000);
	tty << "--Quark Kernel--\n";
	tty << "Low memory: \t" << (int) system_info.getLowMemory() << " KiB\n";
	tty << "High memory:\t" << (int) system_info.getHighMemory() << " KiB\n";
	tty << "Type\t\tLocation\t\tSize\n";
	MemoryMap memmap(&memory_map, 16);
       	for(size_t i = 0; i < memmap.size() && memmap[i].getSize() > 0; i++)
	{
	    tty << (int) memmap[i].getType() << "\t\t\t"
		<< (void*) memmap[i].getLocation() << "\t\t"
		<< (int) memmap[i].getSize() << "\n";
	}
	BuddyAllocator alloc(memmap, (char*) 0xC0000000,
			     system_info.getHighMemory() / 4 + 256, 6);
	AddressSpace vmem(alloc);
	tty << vmem.mmap((void*) 0x80000000, 0x10000) << '\n';
	tty << "Nothing left to do. Hanging.\n";
}

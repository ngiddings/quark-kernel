#include <stddef.h>
#include <stdint.h>

#include "systypes.hpp"
#include "systeminfo.hpp"
#include "mmgr/memorymap.hpp"
#include "mmgr/buddyallocator.hpp"
#include "mmgr/addressspace.hpp"
#include "pio.hpp"
#include "tty.hpp"
#include "util.hpp"
#include "config.h"

using namespace kernel;

extern SystemInfo system_info;
extern MemoryMap::Region memory_map;

void main(char* cmdline)
{
    TTY tty((char*) 0xC00B8000);
	tty << PACKAGE_STRING << "\n\n";
	tty << "Low memory: \t" << (int) system_info.getLowMemory() << " KiB\n";
	tty << "High memory:\t" << (int) system_info.getHighMemory() << " KiB\n";
	tty << "Type\t\tLocation\t\tSize\n";
	MemoryMap memmap(&memory_map, 16);
    for(size_t i = 0; i < memmap.size() && memmap[i].getSize() > 0; i++)
	{
	    tty << (int) memmap[i].getType() << "\t\t\t";
		tty << (void*) memmap[i].getLocation() << "\t\t";
		tty << (int) memmap[i].getSize() << "\n";
	}
	BuddyAllocator alloc(memmap, (char*) 0xC0000000, system_info.getHighMemory() / 4 + 256, 6);
	AddressSpace vmem(alloc);
	vmem.mmap((void*) 0x80000000, 0x10000, MMAP_RW);
	outb(0xa1, 0xff);
	outb(0x21, 0xff);
	tty << "Nothing left to do. Hanging.\n";
}

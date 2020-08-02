#include <stddef.h>
#include <stdint.h>

#include "systypes.hpp"
#include "systeminfo.hpp"
#include "mmgr.hpp"
#include "tty.hpp"
#include "util.hpp"
#include "kernelstate.hpp"
#include "config.h"

using namespace kernel;

BuddyAllocator State::pageAllocator;
Allocator State::allocator;
Interrupts State::interrupts;
ProcessQueue State::processQueue;

extern int _pageMapLocation;
extern int _heapLocation;
extern int _heapSize;

extern SystemInfo system_info;
extern MemoryMap::Region memory_map;

void main(char* cmdline)
{
	MemoryMap memmap(&memory_map, 16);
	State::pageAllocator = BuddyAllocator(memmap, (char*) &_pageMapLocation, system_info.getHighMemory() / 4 + 256, 6);
	mmap((void*) &_heapLocation, 0x100000, MMAP_RW);
	State::allocator = Allocator((void*) 0xFFB00000, 0x100000, 256);

    TTY tty((char*) 0xFF8B8000);
	tty << PACKAGE_STRING << "\n\n";
	tty << "Low memory: \t" << (int) system_info.getLowMemory() << " KiB\n";
	tty << "High memory:\t" << (int) system_info.getHighMemory() << " KiB\n";
	tty << "Type\t\tLocation\t\tSize\n";
    for(size_t i = 0; i < memmap.size() && memmap[i].getSize() > 0; i++)
	{
	    tty << (int) memmap[i].getType() << "\t\t\t";
		tty << (void*) memmap[i].getLocation() << "\t\t";
		tty << (int) memmap[i].getSize() << "\n";
	}
	mmap((void*) 0, 4096, MMAP_RW);
	tty << "Nothing left to do. Hanging.\n";
}

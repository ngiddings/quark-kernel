#include "../mmap.hpp"
#include "pagetableentry.hpp"

int kernel::mmap(MemoryAllocator& allocator, void* start, size_t length, int flags)
{
    if((size_t) start % 4096 != 0)
        return -1;
    PageTableEntry* pageTables = (PageTableEntry*) 0xFFC00000;
    PageTableEntry* pageDirectory = (PageTableEntry*) 0xFFFFF000;
    size_t tableIndex = (size_t) start / 4096;
    for(int i = (int) length; i > 0; i -= 4096)
	{
		size_t directoryIndex = tableIndex / 1024;
		if(!pageDirectory[directoryIndex].getPresent())
		{
			physaddr_t newPT = allocator.allocate(4096);
			if(newPT == 0)
				return -1;
			pageDirectory[directoryIndex] = newPT;
			pageDirectory[directoryIndex].setPresent(true);
			pageDirectory[directoryIndex].setUsermode(false);
			pageDirectory[directoryIndex].setRw(true);
		}
		if(!pageTables[tableIndex].getPresent())
		{
			physaddr_t page = allocator.allocate(4096);
			pageTables[tableIndex] = page;
			pageTables[tableIndex].setPresent(true);
			pageTables[tableIndex].setUsermode(false);
			if(flags & MMAP_RW)
				pageTables[tableIndex].setRw(true);
			
		}
		tableIndex++;
	}
	return 0;
}

int kernel::munmap(MemoryAllocator& allocator, void* start, size_t length)
{
    if((size_t) start % 4096 != 0)
		return -1;
    PageTableEntry* pageTables = (PageTableEntry*) 0xFFC00000;
    PageTableEntry* pageDirectory = (PageTableEntry*) 0xFFFFF000;
	size_t tableIndex = (size_t) start / 4096;
	for(int i = (int) length; i > 0; i -= 4096)
	{
		size_t directoryIndex = tableIndex / 1024;
		if(pageDirectory[directoryIndex].getPresent())
		{
			pageDirectory[directoryIndex] = 0;
			pageDirectory[directoryIndex].setPresent(false);
			pageDirectory[directoryIndex].setUsermode(false);
			pageDirectory[directoryIndex].setRw(false);
		}
		tableIndex++;
	}
	return 0;
}

bool kernel::isMapped(void* addr)
{
    PageTableEntry* pageDirectory = (PageTableEntry*) 0xFFFFF000;
    size_t tableIndex = (size_t) addr / 4096;
    size_t directoryIndex = tableIndex / 1024;
    if(pageDirectory[directoryIndex].getPresent())
    {
        PageTableEntry* pageTables = (PageTableEntry*) 0xFFC00000;
        return pageTables[tableIndex].getPresent();
    }
    return false;
}

physaddr_t kernel::getPhysicalAddress(void* addr)
{
    PageTableEntry* pageTables = (PageTableEntry*) 0xFFC00000;
    size_t tableIndex = (size_t) addr / 4096;
    return pageTables[tableIndex].getPhysicalAddress() + (size_t) addr & 0xFFF;
}

int kernel::createAddressSpace(void* table)
{
	if((size_t) table & 0xFFF != 0)
		return -1;
    PageTableEntry* pageDirectory = (PageTableEntry*) 0xFFFFF000;
    PageTableEntry* newDirectory = (PageTableEntry*) table;
    newDirectory[1022] = pageDirectory[1022];
    newDirectory[1023] = pageDirectory[1023];
}

int kernel::loadAddressSpace(physaddr_t table)
{
	if(table & 0xFFF != 0)
		return -1;
	asm("mov %0, %cr3"
		: 
		: "r" (table));
}
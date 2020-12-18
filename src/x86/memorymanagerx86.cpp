#include "memorymanagerx86.hpp"

unsigned int MemoryManagerx86::getPageSize() const
{
    return pageSize;
}

physaddr_t MemoryManagerx86::createAddressSpace()
{
    if(((size_t) table & 0xFFF) != 0)
		return -1;
    PageTableEntry* newDirectory = (PageTableEntry*) table;
    newDirectory[1022] = m_pageDirectory[1022];
    newDirectory[1023] = m_pageDirectory[1023];
	return 0;
}

void MemoryManagerx86::loadAddressSpace(physaddr_t table)
{

}

int MemoryManagerx86::mapPage(void *page, physaddr_t frame, int flags)
{

}

physaddr_t MemoryManagerx86::unmapPage(void *page)
{

}
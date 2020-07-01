#include "addressspace.h"

qkernel::AddressSpace::AddressSpace(MemoryAllocator& malloc)
  : malloc(malloc)
{
	this->pageTables = (PageTableEntry*) 0xFFC00000;
}

void* qkernel::AddressSpace::mmap(void* start, size_t length)
{
	
}

void qkernel::AddressSpace::munmap(void* start, size_t length)
{

}

void* qkernel::AddressSpace::getPhysicalAddress(void* virtualAddress) const
{
	size_t index = (size_t) virtualAddress / 4096;
	PageTableEntry pte = pageTables[index];
	if(pte.getPresent())
		return (void*) pte.getPhysicalAddress();
	else
		return (void*) 0;
}

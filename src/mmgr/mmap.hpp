#ifndef MMAP_H
#define MMAP_H

#include "memoryallocator.hpp"

#define MMAP_RW 0x01
#define MMAP_EXEC 0x02
#define MMAP_SHARED 0x04

namespace kernel
{

int mmap(MemoryAllocator& allocator, void* start, size_t length, int flags);

int munmap(MemoryAllocator& allocator, void* start, size_t length);

bool isMapped(void* addr);

physaddr_t getPhysicalAddress(void* addr);

int createAddressSpace(void* table);

int loadAddressSpace(physaddr_t table);

}

#endif
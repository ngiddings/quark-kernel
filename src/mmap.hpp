#ifndef MMAP_H
#define MMAP_H

#include "pageallocator.hpp"

#define MMAP_RW 0x01
#define MMAP_EXEC 0x02
#define MMAP_SHARED 0x04

namespace kernel
{

int mmap(void* start, size_t length, int flags);

int mmap(void* start, physaddr_t p_start, size_t length, int flags);

int munmap(void* start, size_t length);

bool isMapped(void* addr);

physaddr_t getPhysicalAddress(void* addr);

int createAddressSpace(void* table);

int loadAddressSpace(physaddr_t table);

}

#endif
#ifndef SRC_ADDRESSSPACE_H_
#define SRC_ADDRESSSPACE_H_

#include <stddef.h>

#include "memoryallocator.hpp"
#include "x86/pagetableentry.hpp"
#include "types.hpp"

#define MMAP_RW 0x01
#define MMAP_EXEC 0x02
#define MMAP_SHARED 0x04

namespace kernel {

class AddressSpace {
public:
    
	AddressSpace(MemoryAllocator& malloc);

	int mmap(void* start, size_t length, int flags);

	int munmap(void* start, size_t length);

	physaddr_t getPhysicalAddress(void* virtualAddress) const;
    
private:

	MemoryAllocator& malloc;

	/**
	 * Array of 1024 page tables, each containing 1024 entries.
	 * The last table represents the page directory.
	 */
	PageTableEntry* pageTables;

	/**
	 * Array of 1024 PDEs, located at the end of the pageTables array
	 */
	PageTableEntry* pageDirectory;

};

} /* namespace kernel */

#endif

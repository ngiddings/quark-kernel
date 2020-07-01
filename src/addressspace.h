#ifndef SRC_ADDRESSSPACE_H_
#define SRC_ADDRESSSPACE_H_

#include <stddef.h>

#include "memoryallocator.h"
#include "pagetableentry.h"

namespace qkernel {

class AddressSpace {
public:
    
	AddressSpace(MemoryAllocator& malloc);

	void* mmap(void* start, size_t length);

	void munmap(void* start, size_t length);

	void* getPhysicalAddress(void* virtualAddress) const;
    
private:

	MemoryAllocator& malloc;

	/**
	 * Array of 1024 page tables, each containing 1024 entries.
	 * The last table represents the page directory.
	 */
	PageTableEntry* pageTables;

};

} /* namespace qkernel */

#endif

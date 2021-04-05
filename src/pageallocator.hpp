#ifndef __MEMORYALLOCATOR_H_
#define __MEMORYALLOCATOR_H_

#include <stddef.h>
#include "systypes.hpp"

namespace kernelns
{

/**
 * Interface for a physical memory allocator.
 */
class PageAllocator
{
public:

	/**
	 * Allocate a single page, returning its physical address. Upon failure,
	 * returns an error code such that the least signifigant byte is nonzero.
	 */
	virtual physaddr_t next() = 0;

	/**
	 * Frees a single page that has previously been allocated.
	 */
	virtual void free(physaddr_t location) = 0;

	/**
	 * @returns the total number of free blocks of memory.
	 */
	virtual size_t freeBlocks() const = 0;

	/**
	 * @returns the total number of blocks managed by this memory 
	 * allocator.
	 */
	virtual size_t getMemorySize() const = 0;
	
};
	
}

#endif

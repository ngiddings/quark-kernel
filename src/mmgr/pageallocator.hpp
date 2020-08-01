#ifndef __MEMORYALLOCATOR_H_
#define __MEMORYALLOCATOR_H_

#include <stddef.h>
#include "types.hpp"

namespace kernel
{

/**
 * Interface for a dymanic memory allocator.
 */
class MemoryAllocator
{
public:

	/**
	 * Allocate a block of memory containing 'size' bytes. May round up
	 * depending on the implementation.
	 */
	virtual physaddr_t allocate(size_t size) = 0;

	/**
	 * Free the region of memory starting at 'location' and containing 
	 * 'size' bytes.
	 */
	virtual void free(physaddr_t location, size_t size) = 0;

	/**
	 * @returns the total number of free blocks of memory.
	 */
	virtual size_t freeBlocks() const = 0;

	/**
	 * @returns the size in blocks of the largest possible allocation that 
	 * will not fail due to lack of memory.
	 */
	virtual size_t maxAllocationSize() const = 0;

	/**
	 * @returns the size in bytes of a single block.
	 */
	virtual size_t getBlockSize() const = 0;

	/**
	 * @returns the total number of blocks managed by this memory 
	 * allocator.
	 */
	virtual size_t getMemorySize() const = 0;
	
};
	
}

#endif

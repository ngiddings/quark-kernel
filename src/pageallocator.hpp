#ifndef __MEMORYALLOCATOR_H_
#define __MEMORYALLOCATOR_H_

#include <stddef.h>
#include "systypes.hpp"
#include "memorymap.hpp"

namespace kernel
{

/**
 * Interface for a dymanic memory allocator.
 */
class PageAllocator
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

class BuddyAllocator : public PageAllocator
{
public:

	BuddyAllocator();

	BuddyAllocator(MemoryMap& memmap, char* bitmap, size_t blockCount,
		       size_t treeHeight);

	BuddyAllocator(char* bitmap, size_t blockSize, size_t blockCount,
		       size_t treeHeight);
    
	/**
	 * Allocate a block of memory containing at least 'size' bytes. 
	 * Rounds up to the nearest power of 2 times the size of a block.
	 */
	virtual physaddr_t allocate(size_t size);

	/**
	 * Free the region of memory starting at 'location' and containing 
	 * 'size' bytes.
	 */
	virtual void free(physaddr_t location, size_t size);

	/**
	 * @returns the total number of free blocks of memory.
	 */
	virtual size_t freeBlocks() const;

	/**
	 * @returns the size in blocks of the largest possible allocation that 
	 * will not fail due to lack of memory.
	 */
	virtual size_t maxAllocationSize() const;

	/**
	 * @returns the size in bytes of a single block.
	 */
	virtual size_t getBlockSize() const;

	/**
	 * @returns the total number of blocks managed by this memory 
	 * allocator.
	 */
	virtual size_t getMemorySize() const;

private:

	static const size_t INVALID = (size_t) -1;

	char* bitmap;

	size_t blockSize;

	size_t blockCount;

	size_t treeHeight;

	size_t findFreeBlock(size_t height);

	size_t split(size_t height, size_t index);

	size_t merge(size_t height, size_t index);

	size_t getBuddy(size_t index);

	size_t getParent(size_t index);

	size_t getChild(size_t index);

	physaddr_t nodeToAddress(size_t height, size_t index) const;

	size_t addressToNode(size_t height, physaddr_t location) const;

	void reserveNode(size_t height, size_t index);

	void freeNode(size_t height, size_t index);

	bool isFree(size_t height, size_t index) const;
	
};
	
}

#endif

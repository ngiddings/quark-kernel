#ifndef BUDDYALLOCATOR_H_
#define BUDDYALLOCATOR_H_

#include "memoryallocator.h"

namespace qkernel
{

class BuddyAllocator : public MemoryAllocator
{
public:

	BuddyAllocator();

	BuddyAllocator(void* heapLocation, char* bitmap, size_t blockSize, size_t blockCount, size_t treeHeight);

	/**
	 * Allocate a block of memory containing at least 'size' bytes. Rounds up to the nearest
	 * power of 2 times the size of a block.
	 */
	virtual void* allocate(size_t size);

	/**
	 * Free the region of memory starting at 'location' and containing 'size' bytes.
	 */
	virtual void free(void* location, size_t size);

	/**
	 * @returns the total number of free blocks of memory.
	 */
	virtual size_t freeBlocks() const;

	/**
	 * @returns the size in blocks of the largest possible allocation that will not
	 * fail due to lack of memory.
	 */
	virtual size_t maxAllocationSize() const;

	/**
	 * @returns the size in bytes of a single block.
	 */
	virtual size_t getBlockSize() const;

	/**
	 * @returns the total number of blocks managed by this memory allocator.
	 */
	virtual size_t getHeapSize() const;

	/**
	 * @returns a pointer to the beginning of the heap managed by this memory allocator.
	 */
	virtual void* getHeapLocation() const;

private:

	static const size_t INVALID = (size_t) -1;

	void* heapLocation;

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

	void* nodeToAddress(size_t height, size_t index) const;

	size_t addressToNode(size_t height, void* location) const;

	void reserveNode(size_t height, size_t index);

	void freeNode(size_t height, size_t index);

	bool isFree(size_t height, size_t index) const;
	
};

}

#endif

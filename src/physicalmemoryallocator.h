/*
 * PhysicalMemoryAllocator.h
 *
 *  Created on: May 23, 2019
 *      Author: nathan
 */

#ifndef PHYSICALMEMORYALLOCATOR_H_
#define PHYSICALMEMORYALLOCATOR_H_

#include <stdint.h>
#include <stddef.h>
#include "bitmap.h"

namespace qkernel {

class PhysicalMemoryAllocator {
public:

	static void* const nullPtr;

	PhysicalMemoryAllocator();

	~PhysicalMemoryAllocator();

	void* allocate(uint32_t pages);

	void free(void* location, uint32_t pages);

	uint32_t totalFreePages();

	uint32_t largestFreeBlock();

private:

	static const uint32_t nullIndex = (uint32_t) -1;

	static const bool available = true;

	static const size_t treeHeight = 20;

	static const size_t treeLeaves = 1024 * 1024;

	Bitmap<treeLeaves> memoryMaps[treeHeight + 1];

	/**
	 * Searches nodes of the given height for an available block. If none is
	 * present, recursively splits higher nodes until one is.
	 *
	 * @param height The height of the desired node.
	 * @returns the index of the located node
	 */
	uint32_t findFreeBlock(uint32_t height);

	/**
	 * Merges a pair of buddies that have both become available. Recurses if
	 * the buddy of the new block is available.
	 *
	 * @param height The height of the blocks to merge.
	 * @param index The index of one of the buddies to be merged.
	 */
	uint32_t merge(uint32_t height, uint32_t index);

	/**
	 * Splits a block into a pair of buddies, making the original unavailable.
	 *
	 * @param height The height of the block to split.
	 * @param index The index of the block to split.
	 *
	 * @returns the index of the first of the pair of new blocks.
	 */
	uint32_t split(uint32_t height, uint32_t index);

	uint32_t getBuddy(uint32_t index);

	uint32_t getParent(uint32_t index);

	uint32_t getChild(uint32_t index);

	uint32_t nodesAtHeight(uint32_t height);

	void* nodeToAddress(uint32_t height, uint32_t index);

	uint32_t addressToNode(uint32_t height, void* address);

};

} /* namespace qkernel */

#endif /* PHYSICALMEMORYALLOCATOR_H_ */

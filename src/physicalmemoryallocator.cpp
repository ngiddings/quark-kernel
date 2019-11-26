/*
 * PhysicalMemoryAllocator.cpp
 *
 *  Created on: May 23, 2019
 *      Author: nathan
 */

#include "physicalmemoryallocator.h"
#include "math.h"
#include "error.h"

namespace qkernel {

void* const PhysicalMemoryAllocator::nullPtr = (void*) -1;

PhysicalMemoryAllocator::PhysicalMemoryAllocator()
{
	for(size_t i = 0; i <= treeHeight; i++)
	{
		memoryMaps[i] = Bitmap<treeLeaves>(!available);
	}
	memoryMaps[treeHeight][0] = available;
}

PhysicalMemoryAllocator::~PhysicalMemoryAllocator()
{
	// TODO Auto-generated destructor stub
}

void* PhysicalMemoryAllocator::allocate(uint32_t pages)
{
	uint32_t height = ilog2(pages, true);
	if(height >= treeHeight) // We can't allocate the whole address space at once!
	{
		return (void*) nullPtr;
	}

	uint32_t index = findFreeBlock(height);
	if(index != nullIndex)
	{
		memoryMaps[height][index] = !available;
		return nodeToAddress(height, index);
	}
	else
	{
		return nullPtr;
	}
}

void PhysicalMemoryAllocator::free(void* location, uint32_t pages)
{
	uint32_t height = ilog2(pages, true);
	if(height >= treeHeight || reinterpret_cast<uint32_t>(location) % (4096 * pages) != 0) // We can't free the whole address space at once!
	{
		return;
	}

	uint32_t index = addressToNode(height, location);
	memoryMaps[height][index] = available;

	if(memoryMaps[height][getBuddy(index)] == available)
	{
		merge(height, index);
	}
}

uint32_t PhysicalMemoryAllocator::totalFreePages()
{
	uint32_t count = 0;
	for(size_t i = 0; i <= treeHeight; i++)
	{
		count += (1 << i) * memoryMaps[i].count();
	}
	return count;
}

uint32_t PhysicalMemoryAllocator::largestFreeBlock()
{
	for(size_t i = treeHeight; i >= 0; i--)
	{
		if(memoryMaps[i].any())
		{
			return 1 << i;
		}
	}
}

uint32_t PhysicalMemoryAllocator::findFreeBlock(uint32_t height)
{
	if(memoryMaps[height].none())
	{
		if(height == treeHeight)
		{
			lastError = Error(ErrorType::outOfMemory, "Out of memory");
			return nullIndex;
		}
		else
		{
			uint32_t parentIndex = findFreeBlock(height + 1);
			if(parentIndex == (uint32_t) -1)
			{
				lastError = Error(ErrorType::outOfMemory, "Out of memory");
				return nullIndex;
			}
			split(height + 1, parentIndex);
		}

	}
	for(size_t index = 0; index < nodesAtHeight(height); index++)
	{
		if(memoryMaps[height][index] == available)
		{
			return index;
		}
	}
	lastError = Error(ErrorType::illegalState, "There were still no free blocks after splitting larger ones!");
	return nullIndex;
}

uint32_t PhysicalMemoryAllocator::merge(uint32_t height, uint32_t index)
{
	if(height == treeHeight)
	{
		lastError = Error(ErrorType::outOfBounds, "Attempted to merge root node");
		return nullIndex;
	}

	if(memoryMaps[height][index] == 0 && memoryMaps[height][getBuddy(index)] == 0)
	{
		memoryMaps[height][index] = !available;
		memoryMaps[height][getBuddy(index)] = !available;
		memoryMaps[height + 1][getParent(index)] = available;
		if(height + 1 < 20)
		{
			if(memoryMaps[height + 1][getBuddy(getParent(index))] == available)
			{
				return merge(height + 1, getParent(index));
			}
		}
		return getParent(index);
	}
	else
	{
		lastError = Error(ErrorType::invalidArgument, "Attempted to merge a node that is in use");
		return nullIndex;
	}
}

uint32_t PhysicalMemoryAllocator::split(uint32_t height, uint32_t index)
{
	if(height == 0)
	{
		lastError = Error(ErrorType::outOfBounds, "Attempted to split leaf node");
		return nullIndex;
	}

	if(memoryMaps[height][index] == available)
	{
		memoryMaps[height][index] = !available;
		memoryMaps[height - 1][getChild(index)] = available;
		memoryMaps[height - 1][getBuddy(getChild(index))] = available;
		return getChild(index);
	}
	else
	{
		lastError = Error(ErrorType::invalidArgument, "Attempted to split node currently in use");
		return nullIndex;
	}
}

uint32_t PhysicalMemoryAllocator::getBuddy(uint32_t index)
{
	return index ^ 1;
}

uint32_t PhysicalMemoryAllocator::getParent(uint32_t index)
{
	return (index - index % 2) / 2;
}

uint32_t PhysicalMemoryAllocator::getChild(uint32_t index)
{
	return index * 2;
}

void* PhysicalMemoryAllocator::nodeToAddress(uint32_t height, uint32_t index)
{
	return reinterpret_cast<void*>(index * 4096 * (1 << height));
}

uint32_t PhysicalMemoryAllocator::nodesAtHeight(uint32_t height)
{
	return treeLeaves / (1 << height);
}

uint32_t PhysicalMemoryAllocator::addressToNode(uint32_t height,
		void* address)
{
	return reinterpret_cast<uint32_t>(address) / (4096 * (1 << height));
}

} /* namespace qkernel */

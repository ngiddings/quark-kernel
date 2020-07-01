#include "buddyallocator.h"
#include "math.h"

#define roundUp(n, m) ((n % m == 0) ? n : (n + m - (n % m)))

qkernel::BuddyAllocator::BuddyAllocator()
{

}

qkernel::BuddyAllocator::BuddyAllocator(void* heapLocation, char* bitmap, size_t blockSize, size_t blockCount, size_t treeHeight)
{
	this->heapLocation = heapLocation;
	this->bitmap = bitmap;
	this->blockSize = blockSize;
	this->blockCount = blockCount;
	this->treeHeight = treeHeight;
	for(size_t i = 0; i <= treeHeight; i++)
	{
		for(size_t j = 0; j < (blockCount >> i); j++)
		{
			if(i < treeHeight)
				reserveNode(i, j);
			else
				freeNode(i, j);
		}
	}
}

void* qkernel::BuddyAllocator::allocate(size_t size)
{
	size_t height = ilog2(roundUp(size, blockSize) / blockSize, true);
	if(height > treeHeight) // Requested block size is greater than maximum
	{
		return NULL;
	}
	else
	{
		size_t index = findFreeBlock(height);
		if(index == INVALID) // Failed to find a big enough free block; out of memory
		{
			return NULL;
		}
		else
		{
			reserveNode(height, index);
			return nodeToAddress(height, index);
		}
	}
}

void qkernel::BuddyAllocator::free(void* location, size_t size)
{
	size_t height = ilog2(roundUp(size, blockSize) / blockSize, true);
	if(height <= treeHeight)
	{
		size_t index = addressToNode(height, location);
		freeNode(height, index);
		if(isFree(height, getBuddy(index)))
		{
			merge(height, index);
		}
	}
}

size_t qkernel::BuddyAllocator::freeBlocks() const
{
	size_t count = 0;
	for(int j = 0; j < blockCount; j++)
	{
		if(isFree(0, j))
		{
			count++;
		}
	}
	return count;
}

size_t qkernel::BuddyAllocator::maxAllocationSize() const
{
	for(int i = treeHeight; i >= 0; i--)
	{
		for(int j = 0; j < (blockCount >> i); j++)
		{
			if(isFree(i, j))
			{
				return 1 << i;
			}
		}
	}
	return 0;
}

size_t qkernel::BuddyAllocator::getBlockSize() const
{
	return blockSize;
}

size_t qkernel::BuddyAllocator::getHeapSize() const
{
	return blockCount;
}

void* qkernel::BuddyAllocator::getHeapLocation() const
{
	return heapLocation;
}

size_t qkernel::BuddyAllocator::findFreeBlock(size_t height)
{
	for(int i = 0; i < (blockCount >> height); i++)
	{
		if(isFree(height, i))
		{
			return i;
		}
	}
	if(height < treeHeight)
	{
		size_t parentIndex = findFreeBlock(height + 1);
		if(parentIndex != INVALID)
		{	
			return split(height + 1, parentIndex);
		}
	}
	return INVALID;
}

size_t qkernel::BuddyAllocator::split(size_t height, size_t index)
{
	if(height > 0 && isFree(height, index))
	{
		reserveNode(height, index);
		freeNode(height - 1, getChild(index));
		freeNode(height - 1, getBuddy(getChild(index)));
		return getChild(index);
	}
	else
	{
		return INVALID;
	}
}

size_t qkernel::BuddyAllocator::merge(size_t height, size_t index)
{
	if(isFree(height, index) && isFree(height, getBuddy(index)) && height < treeHeight)
	{
		reserveNode(height, index);
		reserveNode(height, getBuddy(index));
		freeNode(height + 1, getParent(index));
		if((height + 1) < treeHeight)
		{
			if(isFree(height + 1, getBuddy(getParent(index))))
			{
				return merge(height + 1, getParent(index));

			}
		}
		return getParent(index);
	}
	else
	{
		return INVALID;
	}
}

size_t qkernel::BuddyAllocator::getBuddy(size_t index)
{
	return index ^ 1;
}

size_t qkernel::BuddyAllocator::getParent(size_t index)
{
	return index / 2;
}

size_t qkernel::BuddyAllocator::getChild(size_t index)
{
	return index * 2;
}

void* qkernel::BuddyAllocator::nodeToAddress(size_t height, size_t index) const
{
	char* base = (char*) heapLocation;
	return reinterpret_cast<void*>(base + index * (blockSize << height));
}

size_t qkernel::BuddyAllocator::addressToNode(size_t height, void* location) const
{
	size_t offset = (char*) location - (char*) heapLocation;
	return offset / (blockSize << height);
}

void qkernel::BuddyAllocator::reserveNode(size_t height, size_t index)
{
	size_t bit = (height == 0) ? 0 : ((blockCount * 2) - (blockCount >> (height - 1)));
	bit += index;
	bitmap[bit / 8] |= 1 << (bit % 8);
}

void qkernel::BuddyAllocator::freeNode(size_t height, size_t index)
{
	size_t bit = (height == 0) ? 0 : ((blockCount * 2) - (blockCount >> (height - 1)));
	bit += index;
	bitmap[bit / 8] &= ~(1 << (bit % 8));
}

bool qkernel::BuddyAllocator::isFree(size_t height, size_t index) const
{
	size_t bit = (height == 0) ? 0 : ((blockCount * 2) - (blockCount >> (height - 1)));
	bit += index;
	char data = bitmap[bit / 8] & (1 << (bit % 8));
	if(data == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

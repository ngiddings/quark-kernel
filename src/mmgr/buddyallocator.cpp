#include "buddyallocator.hpp"
#include "types.hpp"
#include "memorymap.hpp"

#define roundUp(n, m) ((n % m == 0) ? n : (n + m - (n % m)))

uint32_t ilog2(uint32_t n, bool roundUp)
{
	uint32_t m = n;
	uint32_t count = 0;
	bool isPowerOfTwo = true;
	while(m)
	{
		if((m & 1) == 1 && m > 1)
		{
			isPowerOfTwo = false;
		}
		count++;
		m >>= 1;
	}
	return count - (isPowerOfTwo ? 1 : (roundUp ? 0 : 1));
}

kernel::BuddyAllocator::BuddyAllocator()
{

}

kernel::BuddyAllocator::BuddyAllocator(kernel::MemoryMap& memmap,
					char* bitmap, size_t blockCount,
					size_t treeHeight)
{
	this->bitmap = bitmap;
	this->blockSize = 4096;
	this->blockCount = blockCount;
	this->treeHeight = treeHeight;
	for(size_t i = 0; i <= treeHeight; i++)
	{
		for(size_t j = 0; j < (blockCount >> i); j++)
		{
			reserveNode(i, j);
		}
	}
	physaddr_t location = 0x100000;
	for(size_t i = 0; i < memmap.size() && memmap[i].getSize() > 0; i++)
	{
		if(memmap[i].getType() != kernel::MemoryMap::AVAILABLE)
			continue;
		if(memmap[i].getLocation() > location)
			location = roundUp(memmap[i].getLocation(), 4096);
		while(memmap[i].contains(location, 4096))
		{
			freeNode(0, location / 4096);
			if(isFree(0, getBuddy(location / 4096)))
				merge(0, location / 4096);
			location += 4096;
		}
	}
}

kernel::BuddyAllocator::BuddyAllocator(char* bitmap, size_t blockSize,
					size_t blockCount, size_t treeHeight)
{
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

physaddr_t kernel::BuddyAllocator::allocate(size_t size)
{
	size_t height = ilog2(roundUp(size, blockSize) / blockSize, true);
	if(height > treeHeight) // Requested block size is greater than maximum
	{
		return 0;
	}
	else
	{
		size_t index = findFreeBlock(height);
		if(index == INVALID) // Failed to find a big enough free block; out of memory
		{
			return 0;
		}
		else
		{
			reserveNode(height, index);
			return nodeToAddress(height, index);
		}
	}
}

void kernel::BuddyAllocator::free(physaddr_t location, size_t size)
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

size_t kernel::BuddyAllocator::freeBlocks() const
{
	size_t count = 0;
	for(size_t j = 0; j < blockCount; j++)
	{
		if(isFree(0, j))
		{
			count++;
		}
	}
	return count;
}

size_t kernel::BuddyAllocator::maxAllocationSize() const
{
	for(size_t i = treeHeight; i >= 0; i--)
	{
		for(size_t j = 0; j < (blockCount >> i); j++)
		{
			if(isFree(i, j))
			{
				return 1 << i;
			}
		}
	}
	return 0;
}

size_t kernel::BuddyAllocator::getBlockSize() const
{
	return blockSize;
}

size_t kernel::BuddyAllocator::getMemorySize() const
{
	return blockCount;
}

size_t kernel::BuddyAllocator::findFreeBlock(size_t height)
{
	for(size_t i = 0; i < (blockCount >> height); i++)
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

size_t kernel::BuddyAllocator::split(size_t height, size_t index)
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

size_t kernel::BuddyAllocator::merge(size_t height, size_t index)
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

size_t kernel::BuddyAllocator::getBuddy(size_t index)
{
	return index ^ 1;
}

size_t kernel::BuddyAllocator::getParent(size_t index)
{
	return index / 2;
}

size_t kernel::BuddyAllocator::getChild(size_t index)
{
	return index * 2;
}

physaddr_t kernel::BuddyAllocator::nodeToAddress(size_t height, size_t index)
    const
{
	return index * (blockSize << height);
}

size_t kernel::BuddyAllocator::addressToNode(size_t height,
					      physaddr_t location) const
{
	return location / (blockSize << height);
}

void kernel::BuddyAllocator::reserveNode(size_t height, size_t index)
{
	size_t bit = (height == 0) ? 0
	    : ((blockCount * 2) - (blockCount >> (height - 1)));
	bit += index;
	bitmap[bit / 8] |= 1 << (bit % 8);
}

void kernel::BuddyAllocator::freeNode(size_t height, size_t index)
{
	size_t bit = (height == 0) ? 0
	    : ((blockCount * 2) - (blockCount >> (height - 1)));
	bit += index;
	bitmap[bit / 8] &= ~(1 << (bit % 8));
}

bool kernel::BuddyAllocator::isFree(size_t height, size_t index) const
{
	size_t bit = (height == 0) ? 0
	    : ((blockCount * 2) - (blockCount >> (height - 1)));
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

#include "allocator.hpp"

#define AVAIL 0
#define UNAVAIL -1

inline size_t ilog2(size_t n)
{
	size_t m = n;
	size_t count = 0;
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
	return count - (isPowerOfTwo ? 1 : 0);
}

kernel::Allocator::Allocator()
{
    
}

kernel::Allocator::Allocator(void* base, size_t heapSize, size_t blockSize)
{
    this->base = (char*) base;
    this->heapSize = heapSize;
    this->blockSize = blockSize;
    this->treeHeight = ilog2(heapSize / blockSize);
    size_t headerSize = (heapSize / blockSize) << 1;
    for(size_t i = 1; i < (heapSize / blockSize) * 2; i++)
        this->base[i] = UNAVAIL;
    for(size_t i = 0; i < heapSize / blockSize; i++)
    {
        if(blockSize * i >= headerSize)
        {
            size_t index = i + (1 << treeHeight);
            this->base[index] = AVAIL;
            for(; index > 1 && this->base[index ^ 1] == 0; index >>= 1)
            {
                this->base[index] = UNAVAIL;
                this->base[index ^ 1] = UNAVAIL;
                this->base[index >> 1] = AVAIL;
            }
        }
        else
        {
            this->base[i + (1 << treeHeight)] = 1;
        }
    }
}

void* kernel::Allocator::allocate(size_t size)
{
    size += blockSize - 1;
    size -= size % blockSize;
    size_t height = ilog2(size / blockSize);
    size_t index = findFreeBlock(height);
    if(index)
    {
        base[index] = height + 1;
        return (void*) ((size_t) base + (blockSize << height) * (index - (1 << (treeHeight - height))));
    }
    return NULL;
}

void kernel::Allocator::free(void* location)
{
    size_t offset = (size_t) location - (size_t) base;
    size_t index = (offset / blockSize) + (1 << treeHeight);
    for(; index > 0 && base[index] == UNAVAIL; index >>= 1);
    base[index] = AVAIL;
    for(; index > 1 && base[index ^ 1] == AVAIL; index >>= 1)
    {
        base[index] = UNAVAIL;
        base[index ^ 1] = UNAVAIL;
        base[index >> 1] = AVAIL;
    }
}

size_t kernel::Allocator::findFreeBlock(size_t height)
{
    if(height > treeHeight)
        return 0;
    for(size_t index = 1 << (treeHeight - height); index < 1 << (treeHeight - height + 1); index++)
    {
        if(base[index] == AVAIL)
            return index;
    }
    size_t index = findFreeBlock(height + 1);
    if(index)
    {
        base[index] = UNAVAIL;
        base[index << 1] = AVAIL;
        base[(index << 1) ^ 1] = AVAIL;
    }
    return index << 1;
}
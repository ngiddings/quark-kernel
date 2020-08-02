#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

namespace kernel
{

class Allocator
{
public:

    Allocator();

    /**
     * @param base A pointer to the start of the heap.
     * @param heapSize The size of the heap, in bytes. Must be a power of two.
     * @param blockSize The smallest unit of allocation, in bytes. Must be a power of two less than or equal to heapSize.
     */
    Allocator(void* base, size_t heapSize, size_t blockSize);

    void* allocate(size_t size);

    void free(void* location);

private:

    size_t findFreeBlock(size_t height);

    char* base;

    size_t heapSize;

    size_t blockSize;

    size_t treeHeight;

};

}

#endif
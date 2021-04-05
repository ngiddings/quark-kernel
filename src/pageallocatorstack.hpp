#ifndef PAGEALLOCATORSTACK_H
#define PAGEALLOCATORSTACK_H

#include "pageallocator.hpp"
#include "memorymap.hpp"

namespace kernelns
{

class PageAllocatorStack : public PageAllocator
{
public:

    PageAllocatorStack(physaddr_t *stackBase, physaddr_t *stackTop, size_t frameSize, const MemoryMap& memoryMap);

    virtual physaddr_t next();

    virtual void free(physaddr_t location);

    virtual size_t freeBlocks() const;

    virtual size_t getMemorySize() const;

private:

    size_t m_totalSize;

    physaddr_t *m_stack, *m_base, *m_top;

};

}

#endif
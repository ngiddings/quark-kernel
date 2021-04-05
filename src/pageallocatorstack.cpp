#include "pageallocatorstack.hpp"
#include "memorytype.hpp"

using namespace kernelns;

PageAllocatorStack::PageAllocatorStack(physaddr_t *stackBase, physaddr_t *stackTop, size_t frameSize, const MemoryMap& memoryMap)
{
    m_base = stackBase;
    m_top = stackTop;
    m_stack = m_base;
    for(int i = 0; i < memoryMap.size(); i++)
    {
        if((MemoryType) memoryMap[i].getType() == MemoryType::Available)
        {
            for(int j = 0; j < memoryMap[i].getSize() / frameSize; j++)
            {
                free(memoryMap[i].getLocation() + j * frameSize);
            }
        }
    }
}

physaddr_t PageAllocatorStack::next()
{
    if(m_stack < m_base)
    {
        physaddr_t frame = *m_stack;
        *m_stack = (physaddr_t) 0;
        m_stack++;
        return frame;
    }
    return (physaddr_t) -1;
}

void PageAllocatorStack::free(physaddr_t location)
{
    if(m_stack > m_top)
    {
        m_stack--;
        *m_stack = location;
    }
}

size_t PageAllocatorStack::freeBlocks() const
{
    return m_base - m_stack;
}

size_t PageAllocatorStack::getMemorySize() const
{
    return m_totalSize;
}
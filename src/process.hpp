#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>
#include "map.hpp"

namespace kernelns
{

class Process
{
public:

    size_t priority;

    void* stack;

    Process();

    bool hasSharedBlock(unsigned int blockID) const;

private:

    Map<unsigned int, MemoryBlock&> m_sharedBlocks;

};

}

#endif
#ifndef SHAREDBLOCK_H
#define SHAREDBLOCK_H

#include "memoryregion.hpp"

class SharedBlock : public MemoryRegion
{
public:

    unsigned int getFlags() const;

private:

    unsigned int m_flags;

};

#endif
#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H

#include "systypes.hpp"

class MemoryBlock
{
public:

    physaddr_t getLocation() const;

    physaddr_t getEnd() const;

    size_t getSize() const;

    int getAttributes() const;

    unsigned int getOwners() const;

    void decrementOwners();

private:

    physaddr_t m_location;

    size_t m_size;

    int m_attributes;

    unsigned int m_owners;

};

#endif
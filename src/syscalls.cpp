#include "syscalls.hpp"
#include "kernel.hpp"

int mmap(void* location, size_t length, int flags)
{
    // TODO: check that the requested region does not overlap something important
    return kernel.allocateRegion(location, length, flags);
}

int munmap(void* location, size_t length)
{
    // TODO: check that the requested region does not overlap something important
    kernel.freeRegion(location, length);
    return 0;
}

unsigned int createSharedBlock(void* location, size_t length, int flags)
{
    unsigned int blockID = kernel.createSharedBlock(length, flags);
    if(blockID > 0)
    {
        const MemoryBlock& block = kernel.getSharedBlock(blockID);
        kernel.mapRegion(location, block.getLocation(), length, flags);
        // TODO: add block to current process
        // TODO: perform safety checks
    }
    return blockID;
}

int aquireSharedBlock(void* location, unsigned int id)
{
    const MemoryBlock& block = kernel.getSharedBlock(id);
    kernel.mapRegion(location, block.getLocation(), block.getSize(), block.getAttributes());
    // TODO: (somehow) handle invalid ids -- possibly hard while using references
    // TODO: add block to current process
    // TODO: perform safety checks
    return 0;
}

int releaseSharedBlock(int id)
{
    // (0) Check that process actually posesses block
    // (1) Get virtual address of block
    // (2) Unmap block
    // (3) Delete block if no one posesses it anymore
    return 0;
}

int querySharedBlock(void* info, int id)
{
    // TODO: define struct for block info
}

int aquirePhysicalBlock(void* location, physaddr_t physicalAddress, size_t length)
{
    return 0;
}

int releasePhysicalBlock(int id)
{
    return 0;
}
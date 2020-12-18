#include "syscalls.hpp"
#include "kernel.hpp"
#include "memorytype.hpp"

int mmap(void* location, size_t length, int flags)
{
    if(kernel.getActiveProcess().map(location, length))
    {
        if(kernel.allocateRegion(location, length, flags))
            return 0;
        else if(kernel.getActiveProcess().unmap(location, length))
            return 1;
        kernel.terminateActiveProcess();
    }
    return kernel.allocateRegion(location, length, flags);
}

int munmap(void* location, size_t length)
{
    if(kernel.getActiveProcess().unmap(location, length))
    {
        kernel.freeRegion(location, length);
        return 0;
    }
    return 1;
}

unsigned int createSharedBlock(void* location, size_t length, int flags)
{
    unsigned int blockID = kernel.createSharedBlock(length, flags);
    if(blockID > 0)
    {
        const SharedBlock& block = kernel.getSharedBlock(blockID);
        kernel.mapRegion(location, block.getLocation(), length, flags);
        // TODO: add block to current process
        // TODO: perform safety checks
    }
    return blockID;
}

int aquireSharedBlock(void* location, unsigned int id)
{
    const SharedBlock& block = kernel.getSharedBlock(id);
    kernel.mapRegion(location, block.getLocation(), block.getSize(), block.getFlags());
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

int sendMessage(unsigned int recipient, const Message* message)
{
    Message* copy = new Message(*message);
    copy->m_sender = kernel.getActiveProcess().getID();
    kernel.getProcess(recipient).pushMessage(copy);
    return 0;
}

int receiveMessage(Message* buffer)
{
    if(buffer == nullptr)
        return 1;
    Message* message = kernel.getActiveProcess().popMessage();
    if(message == nullptr)
        return 1;
    *buffer = *message;
    delete message;
    return 0;
}
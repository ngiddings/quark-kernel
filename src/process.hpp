#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>

#include "message.hpp"

namespace kernelns
{

class Process
{
public:

    size_t priority;

    void* stack;

    Process();

    unsigned int getID() const;

    bool map(void* location, size_t size);

    /**
     * @brief If the specified region is mapped and does not overlap a shared
     * or private block, or the kernel, removes that region from the process's
     * internal memory map. If the specified region overlaps an unmapped region,
     * a shared or private block, or the kernel, the object goes unmodified.
     * This method does not affect the page tables in any way, only the process's
     * internal bookkeeping.
     * 
     * @param location 
     * @param size 
     * @return true if the memory map was sucessfully updated
     * @return false if the memory map was not modified
     */
    bool unmap(void* location, size_t size);

    bool hasSharedBlock(unsigned int blockID) const;

    bool hasPhysicalBlock(unsigned int blockID) const;

    void pushMessage(Message* message);

    Message* popMessage();

private:

};

}

#endif
#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "systypes.h"

class MemoryManager
{
public:

    /**
     * Allocates space for a new top-level page table, and initializes it to
     * point only to the kernel's page tables.
     * 
     * @returns the physical address of the new top-level page table
     */
    virtual physaddr_t createAddressSpace() = 0;

    /**
     * Loads the given top-level page table(s).
     *
     * @param table the top-level page table to load
     */
    virtual void loadAddressSpace(physaddr_t table) = 0;

    /**
     * Maps a single page to a single frame. Allocates a new page table if one
     * does not exist for that frame.
     * 
     * @param page the virtual address of the page to map
     * @param frame the physical address of the frame to map to
     * @param flags flags to apply to the entry
     * 
     * @returns zero upon success, nonzero on failure
     */
    virtual int mapPage(void* page, physaddr_t frame, int flags) = 0;

    /**
     * Deletes the page table entry for the specified page. Does not deallocate
     * redundant page tables or modify higher level tables; these are cleaned up
     * after the owning process ends.
     *
     * @param page the virtual address of the page to unmap
     *
     * @returns zero upon success, nonzero on failure
     */
    virtual int unmapPage(void* page) = 0;

};

#endif
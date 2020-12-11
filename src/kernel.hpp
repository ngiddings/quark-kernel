#ifndef KERNEL_H
#define KERNEL_H

#include "pageallocator.hpp"
#include "memorymanager.hpp"
#include "memorymap.hpp"
#include "memoryblock.hpp"
#include "process.hpp"
#include "systypes.hpp"

using namespace kernelns;

class Kernel
{
public:

    /**
     * @brief Maps a region of pages starting at virtual address 'page' with
     * length 'length'. Allocates each mapped frame and any necessary page 
     * tables. This method does not perform any checks before overwriting page 
     * tables; it is the responsibility of the caller to ensure that the 
     * operation is safe to perform.
     * 
     * @param page The virtual address of the first page to map
     * @param length The size in bytes of the region to map
     * @param flags The flags to apply to each page
     * 
     * @returns zero upon success, nonzero on failure
     */
    int allocateRegion(void* page, size_t length, int flags);

    /**
     * @brief Unmaps and frees a region of mapped pages starting at virtual
     * address 'page' with length 'length'. Does not free any page tables
     * that are made redundant by this operation. It is the responsibility
     * of the caller to ensure that all pages in the specified region are
     * mapped and should be returned to the page allocator.
     * 
     * @param page The virtual address of the first page to free
     * @param length The size in bytes of the region to free
     */
    void freeRegion(void* page, size_t length);

    /**
     * @brief Maps a contiguous region of pages to a contiguous region of 
     * frames. Allocates new page tables as needed. All pages will share 
     * the same flags.
     * 
     * @param page The virtual address of the first page to map
     * @param frame The physical address of the first frame to map to
     * @param length The size in bytes of the region to map
     * @param flags The flags to apply to each page
     * 
     * @returns zero upon success, nonzero on failure
     */
    int mapRegion(void* page, physaddr_t frame, size_t length, int flags);

    /**
     * @brief Unmaps a region of pages; does not return them to the page
     * allocator.
     * 
     * @param page The virtual address of the first page to unmap
     * @param length The size in bytes of the region to unmap
     */
    void unmapRegion(void* page, size_t length);

    unsigned int createSharedBlock(size_t length, int flags);

    const MemoryBlock& getSharedBlock(unsigned int id);

    /**
     * @brief Get the process object corresponsing to 'pid'
     * 
     * @param pid id of the process to fetch
     * @returns a reference to the requested process
     */
    Process& getProcess(unsigned int pid);

    /**
     * @brief Get the current active process object.
     * 
     * @returns a reference to the active process
     */
    Process& getActiveProcess();

    /**
     * @brief Puts the current active process back on the run queue, and sets
     * the active process to the next one selected by the scheduler.
     * 
     * @returns a reference to the new active process
     */
    Process& yieldActiveProcess();

    /**
     * @brief Puts the current active process to sleep, and sets the active
     * process to the next one selected by the scheduler.
     * 
     * @returns a reference to the new active process
     */
    Process& sleepActiveProcess();

    /**
     * @brief Terminates the current active process, freeing all its resources,
     * and activates the next process selected by the scheduler.
     * 
     * @return Process& 
     */
    Process& terminateActiveProcess();

    /**
     * @brief Terminates the process with id 'pid', freeing all its resources.
     * If it holds any shared blocks which are not held by any other process,
     * those blocks will be freed.
     * 
     * @param pid pid of the process to terminate
     */
    void terminateProcess(unsigned int pid);

private:

    PageAllocator& pageAllocator;

    MemoryManager& mmgr;

    MemoryMap& memoryMap;

};

extern Kernel kernel;

#endif
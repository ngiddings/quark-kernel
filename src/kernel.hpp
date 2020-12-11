#ifndef KERNEL_H
#define KERNEL_H

#include "pageallocator.hpp"
#include "memorymanager.hpp"
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

    void deleteSharedBlock(unsigned int id);

    const MemoryBlock& getSharedBlock(unsigned int id);

    const Process& getActiveProcess();

    const Process& switchProcess();

private:

    PageAllocator& pageAllocator;

    MemoryManager& mmgr;

};

extern Kernel kernel;

#endif
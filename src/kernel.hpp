#ifndef KERNEL_H
#define KERNEL_H

#include "systypes.hpp"

class Kernel
{
public:

    Kernel();

    /**
     * Maps a region of pages starting at the virtual address 'page', allocating
     * each mapped frame and any necessary page tables.
     * 
     * @param page The virtual address of the first page to map
     * @param length The size in bytes of the region to map
     * @param flags The flags to apply to each page
     * 
     * @returns zero upon success, nonzero on failure
     */
    int allocateRegion(void* page, size_t length, int flags);

    /**
     * Maps a contiguous region of pages to a configuous region of frames,
     * allocating new page tables as needed. All pages will share the same
     * flags.
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
     * Maps a single page to a single frame. Allocates a new page table if one
     * does not exist for that frame.
     * 
     * @param page the virtual address of the page to map
     * @param frame the physical address of the frame to map to
     * @param flags flags to apply to the entry
     * 
     * @returns zero upon success, nonzero on failure
     */
    int mapPage(void* page, physaddr_t frame, int flags);

    /**
     * Reserve a single page frame.
     * 
     * @returns the physical address of the reserved frame.
     */
    physaddr_t reserveFrame();

    /**
     * Frees a single page frame.
     */
    void freeFrame(physaddr_t frame);

private:



};

#endif
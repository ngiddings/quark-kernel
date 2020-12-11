#include "kernel.hpp"

int Kernel::allocateRegion(void* page, size_t length, int flags)
{
    char* ptr = (char*) page;
    for(int i = (int) length; i > 0; i -= mmgr.getPageSize())
    {
        physaddr_t frame = pageAllocator.allocate(mmgr.getPageSize());
        if(frame != 0)
            mmgr.mapPage(page, reserveFrame(), flags);
        else
            return -1;
        ptr += mmgr.getPageSize();
    }
    return 0;
}

void Kernel::freeRegion(void* page, size_t length)
{
    char* ptr = (char*) page;
    for(int i = (int) length; i > 0; i -= mmgr.getPageSize())
    {
        pageAllocator.free(mmgr.unmapPage((void*) ptr), mmgr.getPageSize());
        ptr += mmgr.getPageSize();
    }
}

int Kernel::mapRegion(void* page, physaddr_t frame, size_t length, int flags)
{

}
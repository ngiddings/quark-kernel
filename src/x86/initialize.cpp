#include <stdint.h>
#include "multiboot2.hpp"
#include "tty.hpp"
#include "../kernelstate.hpp"
#include "../systeminfo.hpp"
#include "../mmap.hpp"
#include "../util.hpp"

using namespace kernel;

extern int _kernelEnd;

enum BootInfoType
{
    Terminate = 0,
    CommandLine = 1,
    BootLoader = 2,
    Module = 3,
    MemoryInfo = 4,
    BootDevice = 5,
    MMap = 6,
    VBEInfo = 7,
    FramebufferInfo = 8,
    ELFSymbols = 9,
    APMTable = 10
};

extern "C"
int startPaging(uint32_t* directory, uint32_t* table, uint32_t* identityTable)
{
    for(int i = 0; i < 1024; i++)
    {
        uint32_t pte = i * 4096 + 3;
        table[i] = pte;
        identityTable[i] = pte;
    }
    directory[0] = ((uint32_t) identityTable) + 3;
    directory[1022] = ((uint32_t) table) + 3;
    directory[1023] = ((uint32_t) directory) + 3;
    asm("mov %0, %%cr3"
        :
        : "r" (directory));
    asm("mov %%cr0, %%eax \n"
        "or $0x80010000, %%eax \n"
        "mov %%eax, %%cr0"
        :
        :
        : "eax");
    return 0;
}

extern "C"
int initialize(void* multibootInfo)
{
    size_t heapSize = 0xFFC00000 - (size_t) &_kernelEnd;
    int log = 0;
    for(; heapSize > 1; log++)
        heapSize >>= 1;
    heapSize <<= log;
    new(&State::allocator) Allocator((void*) (0xFFC00000 - heapSize), heapSize, 64);
    Multiboot2Info bootInfo(multibootInfo);
    if(!bootInfo.isValid())
        return 1;
    bootInfo.getMemoryMap().insertEntry(0, 4 * 1024 * 1024, MemoryMap::UNAVAILABLE);
    new(&State::config) SystemInfo(bootInfo.getMemoryMap(), bootInfo.getCommandLine());
    TTY tty((char*) 0xFF8B8000);
    tty << "Type\t\tLocation\t\tSize\n";
    for(size_t i = 0; i < bootInfo.getMemoryMap().size() && bootInfo.getMemoryMap()[i].getSize() > 0; i++)
	{
	    tty << (int) bootInfo.getMemoryMap()[i].getType() << "\t\t\t";
		tty << (void*) bootInfo.getMemoryMap()[i].getLocation() << "\t\t";
		tty << (int) bootInfo.getMemoryMap()[i].getSize() << "\n";
	}
    // TODO: Initialize page allocator
    // TODO: Initialize process queue, add entry for each module
    return 0;
}

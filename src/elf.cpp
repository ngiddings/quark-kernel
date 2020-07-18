#include "elf.hpp"
#include "util.hpp"

kernel::ELF::ELF(void* location)
{
    this->location = location;
}

void* kernel::ELF::entry()
{
    Header* fileHeader = (Header*) location;
    return fileHeader->entry;
}

int kernel::ELF::validate()
{
    Header* fileHeader = (Header*) location;
    if(fileHeader->magic != 0x464c457f)
        return -1;
    else if((ISA) fileHeader->machine != HOST_ISA)
        return -1;
    else if((Endianness) fileHeader->endianness != Little)
        return -1;
    return 0;
}

int kernel::ELF::load()
{
    Header* fileHeader = (Header*) location;
    ProgramHeader* programHeader = (ProgramHeader*) ((size_t) location + fileHeader->phoffset);
    int count = (int) fileHeader->phcount;
    for(int i = 0; i < count; i++)
    {
        if((SegmentType) programHeader->type != Load)
            continue;
        memcpy(programHeader->vaddr, location + programHeader->offset, programHeader->filesize);
    }
    return 0;
}
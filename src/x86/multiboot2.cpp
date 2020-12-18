#include "multiboot2.hpp"
#include "../memorytype.hpp"

using namespace kernelns;

Multiboot2Info::Multiboot2Info(void* tableLocation)
{
    m_commandLine = NULL;
    m_bootloader = NULL;
    m_moduleCount = 0;
    uint32_t* ptr = (uint32_t*) tableLocation;
    ptr += 2;
    while(*ptr != 0)
    {
        if(*ptr == (uint32_t) Tag::BootCommand)
        {
            m_commandLine = &reinterpret_cast<TagString*>(ptr)->str;
        }
        else if(*ptr == (uint32_t) Tag::Bootloader)
        {
            m_bootloader = &reinterpret_cast<TagString*>(ptr)->str;
        }
        else if(*ptr == (uint32_t) Tag::MemoryMap)
        {
            unsigned int tagSize = reinterpret_cast<TagMemoryMap*>(ptr)->size - 16;
            unsigned int entrySize = reinterpret_cast<TagMemoryMap*>(ptr)->entrySize;
            MemoryMapEntry* entry = &reinterpret_cast<TagMemoryMap*>(ptr)->entries;
            while(tagSize > 0)
            {
                unsigned int regionType = 
                    (entry->type == (unsigned int) Multiboot2MemoryType::Available) ? (unsigned int) MemoryType::Available
                    : ((entry->type == (unsigned int) Multiboot2MemoryType::Defective) ? (unsigned int) MemoryType::Defective
                    : (unsigned int) MemoryType::Unavailable);
                m_memmap.insertEntry(entry->base, entry->length, entry->type);
                entry = (MemoryMapEntry*) (reinterpret_cast<char*>(entry) + entrySize);
                tagSize -= entrySize;
            }
        }
        else if(*ptr == (uint32_t) Tag::Module)
        {
            TagModule* moduleTag = reinterpret_cast<TagModule*>(ptr);
            m_modules[m_moduleCount] = Module(moduleTag->start, moduleTag->end, &moduleTag->str);
            m_memmap.insertEntry(moduleTag->start, moduleTag->end - moduleTag->start, (unsigned int) MemoryType::Unavailable);
        }
        unsigned int size = (ptr[1] + 7) - ((ptr[1] + 7) % 8);
        ptr += size / sizeof(uint32_t);
    }
}

Multiboot2Info::~Multiboot2Info()
{

}

bool Multiboot2Info::isValid() const
{
    return true;
}

MemoryMap& Multiboot2Info::getMemoryMap()
{
    return m_memmap;
}

const Module* Multiboot2Info::getModules() const
{
    return m_modules;
}

unsigned int Multiboot2Info::getModuleCount() const
{
    return m_moduleCount;
}

const char* Multiboot2Info::getCommandLine() const
{
    return m_commandLine;
}

const char* Multiboot2Info::getBootloaderName() const
{
    return m_bootloader;
}
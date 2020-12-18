#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include "../memorymap.hpp"
#include "../module.hpp"

namespace kernelns
{

class Multiboot2Info
{
public:

    enum class Tag
    {
        BootCommand = 1,
        Bootloader = 2,
        Module = 3,
        MemoryInfo = 4,
        BIOSBootDevice = 5,
        MemoryMap = 6,
        VBE = 7,
        Framebuffer = 8,
        ELFSymbols = 9,
        APM = 10,
        EFI32SystemTable = 11,
        EFI64SystemTable = 12,
        SMBIOS = 13,
        ACPI10RSDP = 14,
        ACPT20RSDP = 15,
        Network = 16,
        EFIMemoryMap = 17,
        EFIBootServices = 18,
        EFI32Image = 19,
        EFI64Image = 20,
        LoadAddress = 21
    };

    enum class Multiboot2MemoryType
    {
	    Available = 1,
	    ACPI = 3,
	    Defective = 5
    };

    struct TagString
    {
        uint32_t type;
        uint32_t size;
        char str;
    };

    struct TagModule
    {
        uint32_t type;
        uint32_t size;
        uint32_t start;
        uint32_t end;
        char str;
    };

    struct MemoryMapEntry
    {
        uint64_t base;
        uint64_t length;
        uint32_t type;
    };

    struct TagMemoryMap
    {
        uint32_t type;
        uint32_t size;
        uint32_t entrySize;
        uint32_t entryVersion;
        MemoryMapEntry entries;
    };

    Multiboot2Info(void* tableLocation);

    ~Multiboot2Info();

    bool isValid() const;

    MemoryMap& getMemoryMap();

    const Module* getModules() const;

    unsigned int getModuleCount() const;

    const char* getCommandLine() const;

    const char* getBootloaderName() const;

private:

    Module m_modules[16];

    unsigned int m_moduleCount;

    MemoryMap m_memmap;

    char* m_commandLine;

    char* m_bootloader;

    void* m_tableLocation;

};

}

#endif
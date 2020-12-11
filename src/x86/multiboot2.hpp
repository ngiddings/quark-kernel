#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include "../memorymap.hpp"
#include "../module.hpp"

namespace kernelns
{

class Multiboot2Info
{
public:

    enum TagType
    {
        T_BootCommand = 1,
        T_Bootloader = 2,
        T_Module = 3,
        T_MemoryInfo = 4,
        T_BIOSBootDevice = 5,
        T_MemoryMap = 6,
        T_VBE = 7,
        T_Framebuffer = 8,
        T_ELFSymbols = 9,
        T_APM = 10,
        T_EFI32SystemTable = 11,
        T_EFI64SystemTable = 12,
        T_SMBIOS = 13,
        T_ACPI10RSDP = 14,
        T_ACPT20RSDP = 15,
        T_Network = 16,
        T_EFIMemoryMap = 17,
        T_EFIBootServices = 18,
        T_EFI32Image = 19,
        T_EFI64Image = 20,
        T_LoadAddress = 21
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
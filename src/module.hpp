#ifndef ELF_H
#define ELF_H

#include <stdint.h>

#include "systypes.hpp"

namespace kernel
{

class ELF
{
public:

    enum Endianness
    {
        Little = 1,
        Big = 2
    };

    enum ISA
    {
        NA = 0x00,
        x86 = 0x03,
        MIPS = 0x08,
        PPC = 0x14,
        PPC64 = 0x15,
        ARM = 0x28,
        x86_64 = 0x3E,
        aarch64 = 0xB7
    };

    enum SegmentType
    {
        Unused = 0,
        Load = 1,
        Dynamic = 2
    };

    struct Header
    {
        uint32_t magic;
        char size;
        char endianness;
        char version;
        char abi;
        char abiVersion;
        char reserved[7];
        uint16_t type;
        uint16_t machine;
        uint32_t _version;
        void* entry;
#if defined __i386__ || defined __arm__
        uint32_t phoffset;
        uint32_t shoffset;
#elif defined __x86_64__ || defined __aarch64__
        uint64_t phoffset;
        uint64_t shoffset;
#endif
        uint32_t flags;
        uint16_t headerSize;
        uint16_t phsize;
        uint16_t phcount;
        uint16_t shsize;
        uint16_t shcount;
        uint16_t shstrndx;
    };

    struct ProgramHeader
    {
        uint32_t type;
#if defined __i386__ || defined __arm__
        uint32_t offset;
        void* vaddr;
        physaddr_t paddr;
        uint32_t filesize;
        uint32_t memsize;
        uint32_t flags;
        uint32_t align;
#elif defined __x86_64__ || defined __aarch64__
        uint32_t flags;
        uint64_t offset;
        void* vaddr;
        physaddr_t paddr;
        uint64_t filesize;
        uint64_t memsize;
        uint64_t align;
#endif
    };

    struct SectionHeader
    {
        uint32_t name;
        uint32_t type;
#if defined __i386__ || defined __arm__
        uint32_t flags;
        void* addr;
        uint32_t offset;
        uint32_t size;
        uint32_t link;
        uint32_t info;
        uint32_t align;
        uint32_t entrysize;
#elif defined __x86_64__ || defined __aarch64__
        uint64_t flags;
        void* addr;
        uint64_t offset;
        uint64_t size;
        uint32_t link;
        uint32_t info;
        uint64_t align;
        uint64_t entrysize;
#endif
    };

#if defined __i386__
    static const ISA HOST_ISA = x86;
#elif defined __x86_64__
    static const ISA HOST_ISA = x86_64;
#elif defined __arm__
    static const ISA HOST_ISA = ARM;
#elif defined __aarch64__
    static const ISA HOST_ISA = aarch64;
#endif

    ELF();

    ELF(void* location);

    void* entry();

    int validate();

    int load();

private:

    void* m_fileLocation;

};

class Module
{
public:

    Module();

    Module(physaddr_t start, physaddr_t end);

private:

    physaddr_t m_start;

    physaddr_t m_end;

    ELF binary;

};

}

#endif
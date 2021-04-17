#pragma once

#include "pageallocator.h"
#include "types/physaddr.h"
#include <stdint.h>

enum elf_endianness_t
{
    ELF_LITTLE_ENDIAN = 1,
    ELF_BIG_ENDIAN = 2
};

enum elf_isa_t
{
    ELF_ISA_NA = 0x00,
    ELF_ISA_x86 = 0x03,
    ELF_ISA_MIPS = 0x08,
    ELF_ISA_PPC = 0x14,
    ELF_ISA_PPC64 = 0x15,
    ELF_ISA_ARM = 0x28,
    ELF_ISA_x86_64 = 0x3E,
    ELF_ISA_AARCH64 = 0xB7
};

enum elf_segment_type_t
{
    ELF_UNUSED = 0,
    ELF_LOAD = 1,
    ELF_DYNAMIC = 2
};

struct elf_file_header_t
{
    uint32_t magic;
    char size;
    char endianness;
    char version;
    char abi;
    char abi_version;
    char reserved[7];
    uint16_t type;
    uint16_t machine;
    uint32_t _version;
    void *entry;
#if defined __i386__ || defined __arm__
    uint32_t phoffset;
    uint32_t shoffset;
#elif defined __x86_64__ || defined __aarch64__
    uint64_t phoffset;
    uint64_t shoffset;
#endif
    uint32_t flags;
    uint16_t header_size;
    uint16_t phsize;
    uint16_t phcount;
    uint16_t shsize;
    uint16_t shcount;
    uint16_t shstrndx;
};

struct elf_program_header_t
{
    uint32_t type;
#if defined __i386__ || defined __arm__
    uint32_t offset;
    void *vaddr;
    physaddr_t paddr;
    uint32_t filesize;
    uint32_t memsize;
    uint32_t flags;
    uint32_t align;
#elif defined __x86_64__ || defined __aarch64__
    uint32_t flags;
    uint64_t offset;
    void *vaddr;
    physaddr_t paddr;
    uint64_t filesize;
    uint64_t memsize;
    uint64_t align;
#endif
};

struct elf_section_header_t
{
    uint32_t name;
    uint32_t type;
#if defined __i386__ || defined __arm__
    uint32_t flags;
    void *addr;
    uint32_t offset;
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t align;
    uint32_t entry_size;
#elif defined __x86_64__ || defined __aarch64__
    uint64_t flags;
    void *addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t align;
    uint64_t entry_size;
#endif
};

#if defined __i386__
    static const enum elf_isa_t HOST_ISA = ELF_ISA_x86;
#elif defined __x86_64__
    static const enum elf_isa_t HOST_ISA = ELF_ISA_x86_64;
#elif defined __arm__
    static const enum elf_isa_t HOST_ISA = ELF_ISA_ARM;
#elif defined __aarch64__
    static const enum elf_isa_t HOST_ISA = ELF_ISA_AARCH64;
#endif

int load_program(struct elf_file_header_t *elf, struct page_stack_t *page_stack);

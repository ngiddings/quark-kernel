#pragma once

#include "memorymap.h"
#include "module.h"
#include <stddef.h>
#include <stdint.h>

#define module_limit 8

enum multiboot2_tag_types
{
    MB_END_TAG = 0,
    MB_BOOT_COMMAND = 1,
    MB_BOOTLOADER = 2,
    MB_MODULE = 3,
    MB_MEMORY_INFO = 4,
    MB_BIOS_BOOT_DEVICE = 5,
    MB_MEMORY_MAP = 6,
    MB_VBE = 7,
    MB_FRAMEBUFFER = 8,
    MB_ELF_SYMBOLS = 9,
    MB_APM = 10,
    MB_EFI32_SYSTEM_TABLE = 11,
    MB_EFI64_SYSTEM_TABLE = 12,
    MB_SMBIOS = 13,
    MB_ACPI10_RSDP = 14,
    MB_ACPT20_RSDP = 15,
    MB_NETOWRK = 16,
    MB_EFI_MEMORY_MAP = 17,
    MB_EFI_BOOT_SERVICES = 18,
    MB_EFI32_IMAGE = 19,
    MB_EFI64_IMAGE = 20,
    MB_LOAD_ADDRESS = 21
};

enum multiboot2_memory_types
{
    MB_AVAILABLE = 1,
	MB_ACPI = 3,
	MB_DEFECTIVE = 5
};

struct multiboot2_string_t
{
    uint32_t type;
    uint32_t size;
    char str;
};

struct multiboot2_module_t
{
    uint32_t type;
    uint32_t size;
    uint32_t start;
    uint32_t end;
    char str;
};

struct multiboot2_map_entry_t
{
    uint64_t base;
    uint64_t length;
    uint32_t type;
};

struct multiboot2_memory_map_t
{
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    struct multiboot2_map_entry_t entries;
};

struct boot_info_t
{
    char *bootloader;
    char *parameters;
    size_t module_count;
    struct memory_map_t map;
    struct module_t modules[module_limit];
};

void *read_multiboot_table(struct boot_info_t *boot_info, void *table);

#include "x86/multiboot2.h"
#include "string.h"
#include <stddef.h>
#include <stdint.h>

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

struct multiboot2_memory_info_t
{
    uint32_t type;
    uint32_t size;
    uint32_t low_memory;
    uint32_t high_memory;
};

void *read_multiboot_table_entry(struct boot_info_t *boot_info, void *table)
{
    uint32_t *int_table = (uint32_t *)table;
    switch (*int_table)
    {
    case MB_END_TAG:
        return NULL;
    case MB_MEMORY_INFO:
        boot_info->memory_size = 
            ((struct multiboot2_memory_info_t*) table)->high_memory * 1024 
            + ((struct multiboot2_memory_info_t*) table)->low_memory * 1024;
        break;
    case MB_MEMORY_MAP: ;
        unsigned int tag_size = ((struct multiboot2_memory_map_t*) table)->size - 16;
        unsigned int entry_size = ((struct multiboot2_memory_map_t*) table)->entry_size;
        struct multiboot2_map_entry_t *entry = &((struct multiboot2_memory_map_t*) table)->entries;
        while(tag_size)
        {
            unsigned int entry_type = 
                entry->type == MB_AVAILABLE ? M_AVAILABLE 
                : (entry->type == MB_DEFECTIVE ? M_DEFECTIVE
                : M_UNAVAILABLE);
            memmap_insert_region(&boot_info->map, entry->base, entry->length, entry_type);
            entry = (struct multiboot2_map_entry_t*) ((void*) entry + entry_size);
            tag_size -= entry_size;
        }
        break;
    case MB_MODULE:
        if(boot_info->module_count < 8)
        {
            boot_info->modules[boot_info->module_count].start = ((struct multiboot2_module_t*) table)->start;
            boot_info->modules[boot_info->module_count].end = ((struct multiboot2_module_t*) table)->end;
            strcpy(boot_info->modules[boot_info->module_count].str, &((struct multiboot2_module_t*) table)->str);
            unsigned long size = ((struct multiboot2_module_t*) table)->end - ((struct multiboot2_module_t*) table)->start;
            size += 4095;
            size &= ~4095;
            memmap_insert_region(&boot_info->map, 
                ((struct multiboot2_module_t*) table)->start, 
                size, 
                M_UNAVAILABLE);
            boot_info->module_count++;
        }
        break;
    case MB_BOOT_COMMAND:
        strcpy(boot_info->parameters, &((struct multiboot2_string_t*) table)->str);
        break;
    case MB_BOOTLOADER:
        strcpy(boot_info->bootloader, &((struct multiboot2_string_t*) table)->str);
        break;
    default:
        break;
    }
    size_t size = (int_table[1] + 7) - ((int_table[1] + 7) % 8);
    return table + size;
}

void *read_multiboot_table(struct boot_info_t *boot_info, void *table)
{
    void *multiboot_end = table + *(uint32_t*)table;
    table += 8;
    while (table != NULL)
    {
        table = read_multiboot_table_entry(boot_info, table);
    }
    return multiboot_end;
}

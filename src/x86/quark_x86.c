#include "kernel.h"
#include "pageallocator.h"
#include "memorymap.h"
#include "stdio.h"
#include "string.h"
#include "module.h"
#include <stdint.h>
#include <stddef.h>

#define module_limit 8

enum isr_type_t
{
    INTERRPUT_TASK32 = 5,
    INTERRPUT_TRAP32 = 15,
    INTERRPUT_INT32 = 14,
    INTERRPUT_TRAP16 = 7,
    INTERRPUT_INT16 = 6
};

struct interrupt_descriptor_t
{
    uint16_t m_offset1;
    uint16_t m_selector;
    uint16_t m_zero : 8;
    uint16_t m_type : 4;
    uint16_t m_storage : 1;
    uint16_t m_dpl : 2;
    uint16_t m_present : 1;
    uint16_t m_offset2;
};

struct idt_info_t
{
    uint16_t size;
    void *location;
};

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

extern int _kernelEnd;

struct interrupt_descriptor_t idt[256];

struct page_stack_t page_stack;

struct kernel_t kernel;

void lidt()
{
    struct idt_info_t idt_info;
    idt_info.size = sizeof(idt) - 1;
    idt_info.location = (void *)&idt;
    asm("lidt (%0)"
        :
        : "r"(&idt_info));
}

int startPaging(uint32_t *directory, uint32_t *table, uint32_t *identityTable)
{
    for (int i = 0; i < 1024; i++)
    {
        uint32_t pte = i * 4096 + 3;
        table[i] = pte;
        identityTable[i] = pte;
    }
    directory[0] = ((uint32_t)identityTable) + 3;
    directory[1022] = ((uint32_t)table) + 3;
    directory[1023] = ((uint32_t)directory) + 3;
    asm("mov %0, %%cr3"
        :
        : "r"(directory));
    asm("mov %%cr0, %%eax \n"
        "or $0x80010000, %%eax \n"
        "mov %%eax, %%cr0"
        :
        :
        : "eax");
    return 0;
}

void *read_multiboot_table(struct boot_info_t *boot_info, void *table)
{
    uint32_t *int_table = (uint32_t *)table;
    switch (*int_table)
    {
    case MB_END_TAG:
        return NULL;
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
            insert_region(&boot_info->map, entry->base, entry->length, entry_type);
            entry = (struct multiboot2_map_entry_t*) ((void*) entry + entry_size);
            tag_size -= entry_size;
        }
        break;
    case MB_MODULE:
        if(boot_info->module_count < 8)
        {
            boot_info->modules[boot_info->module_count].start = ((struct multiboot2_module_t*) table)->start;
            boot_info->modules[boot_info->module_count].end = ((struct multiboot2_module_t*) table)->end;
            strcpy(boot_info->modules[boot_info->module_count].str, ((struct multiboot2_module_t*) table)->str);
            insert_region(&boot_info->map, 
                ((struct multiboot2_module_t*) table)->start, 
                ((struct multiboot2_module_t*) table)->end - ((struct multiboot2_module_t*) table)->start, 
                M_UNAVAILABLE);
            boot_info->module_count++;
        }
        else
        {
            printf("WARNING: Too many modules, must skip one.\n");
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

int initialize(void *multiboot_info)
{
    struct memory_region_t map_array[16];
    char bootloader_name[64];
    char kernel_parameters[64];
    struct boot_info_t boot_info = {
        .bootloader = bootloader_name,
        .parameters = kernel_parameters,
        .module_count = 0,
        .map = {
            .array = map_array,
            .size = 0,
            .capacity = 16}};
    multiboot_info += 8;
    while (multiboot_info != NULL)
    {
        multiboot_info = read_multiboot_table(&boot_info, multiboot_info);
    }
    insert_region(&boot_info.map, 0, 1 << 22, M_UNAVAILABLE);
    printf("Type\t\tLocation\t\tSize\n");
    for (size_t i = 0; i < boot_info.map.size && boot_info.map.array[i].size > 0; i++)
    {
        printf("%i\t\t\t%08X\t\t%i\n", boot_info.map.array[i].type, boot_info.map.array[i].location, boot_info.map.array[i].size);
    }
    page_stack.base_pointer = 0xFFC00000;
    page_stack.stack_pointer = 0xFFC00000;
    page_stack.limit_pointer = 0xFF900000;
    initialize_page_stack(&page_stack, &boot_info.map, 4096);
    
    // TODO: Initialize process queue
    for(int i = 0; i < boot_info.module_count; i++)
    {
        load_module(&kernel, &boot_info.modules[i]);
    }
    // TODO: enter first process
    putchar('&');
}

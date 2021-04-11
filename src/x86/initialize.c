#include "memorymap.h"
#include "tty.h"
#include "types/memorytype.h"
#include <stdint.h>
#include <stddef.h>

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

struct boot_info_t
{
    struct memory_map_t map;
    char *bootloader;
    char *parameters;
};

extern int _kernelEnd;

struct interrupt_descriptor_t idt[256];

struct idt_info_t
{
    uint16_t size;
    void *location;
};

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
}

int initialize(void *multiboot_info)
{
    struct memory_region_t map_array[16];
    struct boot_info_t boot_info = {
        .map = {
            .array = map_array, 
            .size = 0, 
            .capacity = 16
        }, 
        .bootloader = NULL, 
        .parameters = NULL
    };
    while(multiboot_info != NULL)
    {
        multiboot_info = read_multiboot_table(&boot_info, multiboot_info);
    }
    insert_region(&boot_info.map, 0, 1 << 22, M_UNAVAILABLE);
    printf("Type\t\tLocation\t\tSize\n");
    for (size_t i = 0; i < boot_info.map.size && boot_info.map.array[i].size > 0; i++)
    {
        printf("%i\t\t\t%08X\t\t%i\n", boot_info.map.array[i].type, boot_info.map.array[i].location, boot_info.map.array[i].size);
    }
    // TODO: Initialize page allocator
    // TODO: Initialize process queue, add entry for each module
    return 0;
}

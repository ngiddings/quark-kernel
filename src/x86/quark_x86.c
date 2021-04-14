#include "kernel.h"
#include "pageallocator.h"
#include "multiboot2.h"
#include "memorymap.h"
#include "stdio.h"
#include "string.h"
#include "module.h"
#include <stdint.h>
#include <stddef.h>

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



extern int _kernelEnd;

void lidt(struct interrupt_descriptor_t *idt)
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

int initialize(void *multiboot_info)
{
    static struct interrupt_descriptor_t idt[256];
    static struct page_stack_t page_stack;
    static struct kernel_t kernel;
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
        printf("%i\t\t\t%08x\t\t%u\n", boot_info.map.array[i].type, boot_info.map.array[i].location, boot_info.map.array[i].size);
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
    // TODO: setup IDT

    // TODO: enter first process
}

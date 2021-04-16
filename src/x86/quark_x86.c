#include "kernel.h"
#include "pageallocator.h"
#include "multiboot2.h"
#include "memorymap.h"
#include "apic.h"
#include "interrupts.h"
#include "msr.h"
#include "stdio.h"
#include "string.h"
#include "module.h"
#include "isr.h"
#include "config.h"
#include <stdint.h>
#include <stddef.h>

extern int _kernel_end;

struct apic_registers_t volatile *apic_registers;

int start_paging(uint32_t *directory, uint32_t *table, uint32_t *identityTable)
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
    initialize_gdt();
    initialize_idt();
    printf("***%s***\n", PACKAGE_STRING);
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
    page_stack.base_pointer = (physaddr_t*)0xFFC00000;
    page_stack.stack_pointer = (physaddr_t*)0xFFC00000;
    page_stack.limit_pointer = (physaddr_t*)0xFF900000;
    initialize_page_stack(&page_stack, &boot_info.map, 4096);
    for(int i = 0; i < boot_info.module_count; i++)
    {
        load_module(&kernel, &boot_info.modules[i]);
    }
    apic_enable(page_stack);
    apic_registers->divide_config.value = APIC_DIVIDE_128;
    apic_registers->lvt_timer.vector = ISR_APIC_TIMER;
    apic_registers->lvt_timer.timer_mode = APIC_TIMER_PERIODIC;
    apic_registers->initial_count.value = 1024*1024*1024;
    apic_registers->lvt_timer.mask = 0;
    printf("spurious_interrupt_vector: %08x\n", *((uint32_t*) &apic_registers->spurious_iv));
    printf("lvt_timer_vector: %08x\n", *((uint32_t*) &apic_registers->lvt_timer));
    asm("sti");
    while(1)
    {
        asm("hlt");
    }
    // TODO: enter first process
}

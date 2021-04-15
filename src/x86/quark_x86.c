#include "kernel.h"
#include "pageallocator.h"
#include "multiboot2.h"
#include "memorymap.h"
#include "apic.h"
#include "interrupts.h"
#include "stdio.h"
#include "string.h"
#include "module.h"
#include "isr.h"
#include <stdint.h>
#include <stddef.h>

extern int _kernelEnd;

struct apic_registers_t volatile *apic_registers;

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
    page_stack.base_pointer = (physaddr_t*)0xFFC00000;
    page_stack.stack_pointer = (physaddr_t*)0xFFC00000;
    page_stack.limit_pointer = (physaddr_t*)0xFF900000;
    initialize_page_stack(&page_stack, &boot_info.map, 4096);
    for(int i = 0; i < boot_info.module_count; i++)
    {
        load_module(&kernel, &boot_info.modules[i]);
    }
    // TODO: setup IDT
    memset(idt, 0, sizeof(struct interrupt_descriptor_t) * 256);
    create_interrupt_descriptor(&idt[EXCEPTION_DIV_BY_0], (void*)isr_division_by_zero, INTERRPUT_INT32, 3);
    create_interrupt_descriptor(&idt[EXCEPTION_GPF], (void*)isr_gp_fault, INTERRPUT_INT32, 3);
    create_interrupt_descriptor(&idt[EXCEPTION_PAGE_FAULT], (void*)isr_page_fault, INTERRPUT_INT32, 3);
    create_interrupt_descriptor(&idt[EXCEPTION_DOUBLE_FAULT], (void*)isr_double_fault, INTERRPUT_INT32, 3);
    create_interrupt_descriptor(&idt[0x80], (void*)isr_syscall, INTERRPUT_INT32, 3);

    // TODO: setup APIC
    asm volatile(
        "mov $0xFF, %%al;"
        "outb %%al, $0xA1;"
        "outb %%al, $0x21;"
        ::: "al"
    );
    apic_enable();
    // TODO: enter first process
}

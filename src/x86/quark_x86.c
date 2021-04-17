#include "kernel.h"
#include "pageallocator.h"
#include "allocator.h"
#include "mmgr.h"
#include "priorityqueue.h"
#include "multiboot2.h"
#include "memorymap.h"
#include "apic.h"
#include "interrupts.h"
#include "stdio.h"
#include "string.h"
#include "module.h"
#include "config.h"
#include <stdint.h>
#include <stddef.h>

extern int _kernel_pstart;
extern int _kernel_pend;
extern int _kernel_start;
extern int _kernel_end;

struct kernel_t kernel_state;

int initialize(void *multiboot_info)
{
    initialize_gdt();
    initialize_idt();
    initialize_allocator(&_kernel_end, (void*)0xFFC00000);
    static struct page_stack_t page_stack;
    struct memory_region_t map_array[24];
    char bootloader_name[64];
    char kernel_parameters[64];
    struct boot_info_t boot_info = {
        .bootloader = bootloader_name,
        .parameters = kernel_parameters,
        .module_count = 0,
        .map = {
            .array = map_array,
            .size = 0,
            .capacity = 24}};
    void *multiboot_end = multiboot_info + *(uint32_t*)multiboot_info;
    multiboot_info += 8;
    while (multiboot_info != NULL)
    {
        multiboot_info = read_multiboot_table(&boot_info, multiboot_info);
    }
    insert_region(&boot_info.map, (physaddr_t)&_kernel_pstart, (physaddr_t)&_kernel_pend - (physaddr_t)&_kernel_pstart, M_UNAVAILABLE);
    for(void *p = (void*)&_kernel_end; p < multiboot_end; p += page_size)
    {
        unmap_page(p);
    }
    initialize_screen();
    printf("***%s***\n", PACKAGE_STRING);
    printf("Type\t\tLocation\t\tSize\n");
    for (size_t i = 0; i < boot_info.map.size && boot_info.map.array[i].size > 0; i++)
    {
        printf("%i\t\t\t%08x\t\t%u\n", boot_info.map.array[i].type, boot_info.map.array[i].location, boot_info.map.array[i].size);
    }
    page_stack.base_pointer = (physaddr_t*)0xFFC00000;
    page_stack.stack_pointer = (physaddr_t*)0xFFC00000;
    page_stack.limit_pointer = (physaddr_t*)0xFFC00000;
    initialize_page_stack(&page_stack, &boot_info.map);
    apic_enable(page_stack);
    apic_registers->divide_config.value = APIC_DIVIDE_128;
    apic_registers->lvt_timer.vector = ISR_APIC_TIMER;
    apic_registers->lvt_timer.timer_mode = APIC_TIMER_PERIODIC;
    apic_registers->initial_count.value = 1024*1024*1024;
    apic_registers->lvt_timer.mask = 0;

    
    static struct priority_queue_t priority_queue;
    construct_priority_queue(&priority_queue, &page_stack);
    static struct resource_table_t resource_table;
    construct_resource_table(&resource_table, &page_stack);
    kernel_state.page_stack = &page_stack;
    kernel_state.resource_table = &resource_table;
    kernel_state.priority_queue = &priority_queue;
    kernel_state.active_process = NULL;
    for(int i = 0; i < boot_info.module_count; i++)
    {
        load_module(&kernel_state, &boot_info.modules[i]);
    }
    next_process(&kernel_state, NULL);

    asm("sti");
    while(1)
    {
        asm("hlt");
    }
    // next_process(&kernel_state, NULL);
}

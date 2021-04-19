#include "kernel.h"
#include "pageallocator.h"
#include "allocator.h"
#include "mmgr.h"
#include "priorityqueue.h"
#include "x86/multiboot2.h"
#include "memorymap.h"
#include "x86/apic.h"
#include "x86/interrupts.h"
#include "stdio.h"
#include "string.h"
#include "module.h"
#include "system.h"
#include "syscalls.h"
#include "config.h"
#include <stdint.h>
#include <stddef.h>

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
    /*printf("Type\t\tLocation\t\tSize\n");
    for (size_t i = 0; i < boot_info.map.size && boot_info.map.array[i].size > 0; i++)
    {
        printf("%i\t\t\t%08x\t\t%u\n", boot_info.map.array[i].type, boot_info.map.array[i].location, boot_info.map.array[i].size);
    }*/
    initialize_page_stack(&page_stack, &boot_info.map, (physaddr_t*)0xFFC00000);
    static struct priority_queue_t priority_queue;
    construct_priority_queue(&priority_queue, &page_stack);
    static struct resource_table_t resource_table;
    construct_resource_table(&resource_table, &page_stack);
    construct_kernel_state(&kernel_state, &page_stack, &priority_queue, &resource_table, boot_info.module_count, boot_info.modules);
    memset(syscall_table, 0, sizeof(syscall_t)*32);
    syscall_table[SYSCALL_TEST] = test_syscall;
    syscall_table[SYSCALL_MMAP] = mmap;
    syscall_table[SYSCALL_MUNMAP] = munmap;
    apic_enable(page_stack);
    /*apic_registers->divide_config.value = APIC_DIVIDE_1;
    apic_registers->lvt_timer.timer_mode = APIC_TIMER_PERIODIC;
    apic_registers->lvt_timer.vector = ISR_PREEMPT;
    apic_registers->lvt_timer.mask = 0;
    apic_registers->initial_count.value = 1024*1024;*/
    asm("sti");
    load_context(next_process(&kernel_state, NULL));
    while(1) asm("hlt");
    
}

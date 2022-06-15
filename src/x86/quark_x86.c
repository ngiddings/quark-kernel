#include "kernel.h"
#include "mmgr.h"
#include "x86/multiboot2.h"
#include "x86/idt.h"
#include "system.h"

void x86_startup(void *multiboot_info)
{
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
    initialize_gdt();
    initialize_idt();
    void *multiboot_end = read_multiboot_table(&boot_info, multiboot_info);
    for(void *p = (void*)&_kernel_end; p < multiboot_end; p += page_size)
    {
        unmap_page(p);
    }
    kernel_initialize(&boot_info);
}

#include "multiboot2.h"
#include "stdio.h"
#include "string.h"

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

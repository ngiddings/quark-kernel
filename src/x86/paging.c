#include "platform/paging.h"
#include "string.h"

#include <stddef.h>

struct page_table_entry_t
{
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t usermode : 1;
    uint32_t write_through : 1;
    uint32_t cache_disable : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t pat : 1;
    uint32_t global : 1;
    uint32_t shared : 1;
    uint32_t type : 2;
    uint32_t physical_address : 20;
};

extern int _kernel_start;

const size_t page_size = 4096;

const size_t page_bits = 12;

const size_t page_table_levels = 2;

struct page_table_entry_t *page_tables = (struct page_table_entry_t *)0xFFC00000;

struct page_table_entry_t *page_directory = (struct page_table_entry_t *)0xFFFFF000;

struct page_table_entry_t *get_pte_pointer(void *page, int level)
{
    unsigned int directory_index = (unsigned int)page >> 22;
    struct page_table_entry_t *entry = NULL;
    if(level == 0)
    {
        entry = &page_directory[directory_index];
    }
    else if(level == 1 && page_directory[directory_index].present)
    {
        unsigned int page_index = (unsigned int)page >> page_bits;
        entry = &page_tables[page_index];
    }
    return entry;
}

int start_paging(void *linear_addr, physaddr_t start, physaddr_t end, uint32_t *directory, uint32_t *table, uint32_t *identity_table)
{
    unsigned int directory_index = (unsigned int) linear_addr >> 22;
    physaddr_t p = start;
    size_t count = 0;
    while (p < end)
    {
        uint32_t table_entry = p + 3;
        int index = p / page_size;
        table[index - start / page_size] = table_entry;
        identity_table[index] = table_entry;
        p += page_size;
        count++;
    }
    directory[0] = ((uint32_t)identity_table) + 3;
    directory[directory_index] = ((uint32_t)table) + 3 + 1024;
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
    return count;
}

int paging_init_top_table(physaddr_t table)
{
    struct page_table_entry_t buffer = page_directory[0];
    page_directory[0].physical_address = table >> page_bits;
    asm volatile("invlpg 0xFFC00000" ::
                     : "memory");
    memset((void *)page_tables, 0, page_size);
    int last_index = (page_size / sizeof(struct page_table_entry_t)) - 1;
    int kernel_index = (size_t)&_kernel_start >> 22;
    for(int i = kernel_index; i < last_index; i++)
    {
        page_tables[i] = page_directory[i];
    }
    page_tables[last_index].physical_address = table >> page_bits;
    page_tables[last_index].present = 1;
    page_tables[last_index].rw = 1;
    page_directory[0] = buffer;
    asm volatile("invlpg 0xFFC00000" ::
                     : "memory");
    return 0;
}

physaddr_t paging_current_address_space()
{
    return page_directory[1023].physical_address << page_bits;
}

void paging_load_address_space(physaddr_t table)
{
    asm volatile("mov %0, %%cr3"
                 :
                 : "r"(table)
                 : "memory");
}

int get_pte_type(void *page, int level)
{
    struct page_table_entry_t *entry = get_pte_pointer(page, level);
    if(entry != NULL)
    {
        int flags = (entry->present ? PAGE_PRESENT | PAGE_EXECUTABLE : 0)
            | (entry->rw ? PAGE_RW : 0)
            | (entry->usermode ? PAGE_USERMODE : 0);
        return flags;
    }
    else
    {
        return 0;
    }
}

int set_pte_type(void *page, int level, int flags)
{
    struct page_table_entry_t *entry = get_pte_pointer(page, level);
    if(entry != NULL)
    {
        entry->present = PAGE_PRESENT ? 1 : 0;
        entry->rw = PAGE_RW ? 1 : 0;
        entry->usermode = PAGE_USERMODE ? 1 : 0;
        return 0;
    }
    else
    {
        return -1;
    }
}

physaddr_t get_pte_address(void *page, int level)
{
    struct page_table_entry_t *entry = get_pte_pointer(page, level);
    if(entry != NULL)
    {
        return entry->physical_address << page_bits | ((size_t)page & 0xFFF);
    }
    else
    {
        return -1;
    }
}

int set_pte_address(void *page, int level, physaddr_t addr)
{
    struct page_table_entry_t *entry = get_pte_pointer(page, level);
    if(entry != NULL)
    {
        entry->physical_address = addr >> page_bits;
        return 0;
    }
    else
    {
        return -1;
    }
}

int set_pte(void *page, int level, int flags, physaddr_t addr)
{
    if(set_pte_address(page, level, addr) == 0)
    {
        return set_pte_type(page, level, flags);
    }
    else
    {
        return -1;
    }
}

void wipe_page_table(void *page, int level)
{
    if(level == 1 && (get_pte_type(page, 0) & PAGE_PRESENT) != 0)
    {
        unsigned int table_index = ((unsigned int)page >> page_bits) & ~0x3FF;
        for(int i = 0; i < 1024; i++)
        {
            page_tables[table_index + i].present = 0;
            page_tables[table_index + i].physical_address = 0;
        }
    }
}

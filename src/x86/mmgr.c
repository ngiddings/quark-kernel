#include "mmgr.h"
#include "pageallocator.h"
#include "string.h"
#include "types/status.h"
#include <stdint.h>

const size_t page_size = 4096;

const size_t page_bits = 12;

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
    uint32_t ignored : 2;
    uint32_t physical_address : 20;
};

struct page_table_entry_t *page_tables = (struct page_table_entry_t *)0xFFC00000;

struct page_table_entry_t *page_directory = (struct page_table_entry_t *)0xFFFFF000;

int start_paging(physaddr_t start, physaddr_t end, uint32_t *directory, uint32_t *table, uint32_t *identity_table)
{
    physaddr_t p = start;
    size_t count = 0;
    while(p < end)
    {
        uint32_t table_entry = p + 3;
        int index = p / page_size;
        table[index - start / page_size] = table_entry;
        identity_table[index] = table_entry;
        p += page_size;
        count++;
    }
    directory[0] = ((uint32_t)identity_table) + 3;
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
    return count;
}

physaddr_t create_address_space(struct page_stack_t *page_stack)
{
    physaddr_t table = reserve_page(page_stack);
    if (table == S_OUT_OF_MEMORY)
    {
        return S_OUT_OF_MEMORY;
    }
    struct page_table_entry_t buffer = page_directory[0];
    page_directory[0].physical_address = table >> page_bits;
    asm volatile("invlpg 0xFFC00000" ::
                     : "memory");
    memset((void *)page_tables, 0, 1022 * 4);
    page_tables[1022] = page_directory[1022];
    page_tables[1023].physical_address = table >> page_bits;
    page_tables[1023].present = 1;
    page_tables[1023].rw = 1;
    page_directory[0] = buffer;
    asm volatile("invlpg 0xFFC00000" ::
                     : "memory");
    return table;
}

void load_address_space(physaddr_t table)
{
    asm volatile("mov %0, %%cr3"
                 :
                 : "r"(table)
                 : "memory");
}

physaddr_t current_address_space()
{
    return page_directory[1023].physical_address << 12;
}

int map_page(struct page_stack_t *page_stack, void *page, physaddr_t frame, int flags)
{
    if ((size_t)page % page_size != 0 || frame % page_size != 0)
    {
        return S_OUT_OF_BOUNDS;
    }
    size_t table_index = (size_t)page / page_size;
    size_t directory_index = table_index / (page_size / sizeof(struct page_table_entry_t));
    if (!page_directory[directory_index].present)
    {
        physaddr_t new_table = reserve_page(page_stack);
        if (new_table == S_OUT_OF_MEMORY)
        {
            return S_OUT_OF_MEMORY;
        }
        page_directory[directory_index].physical_address = new_table >> page_bits;
        page_directory[directory_index].present = 1;
        page_directory[directory_index].usermode = (directory_index < 1022) ? 1 : 0;
        page_directory[directory_index].rw = 1;
    }
    page_tables[table_index].physical_address = frame >> 12;
    page_tables[table_index].present = 1;
    page_tables[table_index].usermode = (flags & PAGE_USERMODE) ? 1 : 0;
    page_tables[table_index].rw = (flags & PAGE_RW) ? 1 : 0;
    asm volatile("invlpg (%0)"
                 :
                 : "r"(page)
                 : "memory");
    return S_OK;
}

physaddr_t unmap_page(void *page)
{
    if ((size_t)page % page_size != 0)
    {
        return S_OUT_OF_BOUNDS;
    }
    size_t table_index = (size_t)page / page_size;
    size_t directory_index = table_index / (page_size / sizeof(struct page_table_entry_t));
    if (!page_directory[directory_index].present || !page_tables[table_index].present)
    {
        return S_OUT_OF_BOUNDS;
    }
    else
    {
        physaddr_t frame = page_tables[table_index].physical_address << page_bits;
        memset(&page_tables[table_index], 0, sizeof(struct page_table_entry_t));
        asm volatile("invlpg (%0)"
                     :
                     : "r"(page)
                     : "memory");
        return frame;
    }
}
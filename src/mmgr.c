#include "mmgr.h"
#include "string.h"
#include "math.h"
#include "platform/paging.h"
#include "types/status.h"
#include "stdio.h"
#include <libmalloc/bitmap_alloc.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_CACHE_SIZE 32

bitmap_heap_descriptor_t page_map;

physaddr_t reserve_pages(size_t size)
{
    unsigned long location = reserve_region(&page_map, size);
    if(location != NOMEM)
    {
        return location;
    }
    else
    {
        return ENOMEM;
    }
}

int free_pages(physaddr_t location, size_t size)
{
    free_region(&page_map, location, size);
    return ENONE;
}

physaddr_t reserve_page()
{
    return reserve_region(&page_map, page_size);
}

int free_page(physaddr_t location)
{
    free_region(&page_map, location, page_size);
    return ENONE;
}

size_t free_page_count()
{
    return page_map.free_block_count;
}

void *page_map_base()
{
    return (void*)page_map.bitmap;
}

void *page_map_end()
{
    return (void*)page_map.bitmap + page_map.bitmap_size;
}

error_t initialize_page_map(memory_map_t *map, void *base, size_t memory_size, unsigned long block_size)
{
    static unsigned long page_map_cache[MAX_CACHE_SIZE];
    // Round memory_size up to nearest power of 2
    memory_size = 1 << llog2(memory_size);

    page_map.block_size = block_size;
    page_map.cache = page_map_cache;
    page_map.cache_capacity = MAX_CACHE_SIZE;
    page_map.bitmap = (unsigned long*) base;

    /* Allocate pages for bitmap */
    int pages_mapped = 0;
    int pages_needed = (bitmap_size(map, block_size) + page_size - 1) / page_size;
    for(int i = 0; i < map->size && (pages_mapped < pages_needed); i++)
    {
        if(map->array[i].type != M_AVAILABLE)
        {
            continue;
        }
        physaddr_t location = (map->array[i].location + page_size - 1) & ~(page_size - 1);
        physaddr_t region_end = map->array[i].location + map->array[i].size;
        while(location + page_size <= region_end && (pages_mapped < pages_needed))
        {
            void *page = (void*)page_map.bitmap + pages_mapped * page_size;
            for(int level = 0; level < page_table_levels; level++)
            {
                if(!(get_pte_type(page, level) & PAGE_PRESENT))
                {
                    if(set_pte(page, level, PAGE_PRESENT | PAGE_RW, location))
                    {
                        return ENOMEM;
                    }
                    else if(level == page_table_levels - 1)
                    {
                        pages_mapped++;
                    }
                    break;
                }
                else if(level == page_table_levels - 1)
                {
                    pages_mapped++;
                }
            }
            memmap_insert_region(map, location, page_size, M_UNAVAILABLE);
            location += page_size;
            continue;
        }
    }
 
    if(initialize_physical_heap(&page_map, map))
    {
        return ENOMEM;
    }
    else
    {
        return ENONE;
    }
}

physaddr_t create_address_space()
{
    physaddr_t table = reserve_page();
    int result;
    if (table == ENOMEM)
    {
        return ENOMEM;
    }
    else if((result = paging_init_top_table(table)))
    {
        return result;
    }
    else
    {
        return table;
    }
}

physaddr_t current_address_space()
{
    return paging_current_address_space();
}

int map_page(void *page, physaddr_t frame, int flags)
{
    if (frame % page_size != 0)
    {
        return EINVALIDARG;
    }
    for(int level = 0; level < page_table_levels - 1; level++)
    {
        int present = get_pte_type(page, level) & PAGE_PRESENT;
        if(present == 0)
        {
            physaddr_t new_table = reserve_page();
            if(new_table == ENOMEM)
            {
                return ENOMEM;
            }
            set_pte(page, level, PAGE_PRESENT | PAGE_USERMODE | PAGE_RW, new_table);
            wipe_page_table(page, level + 1);
        }
    }
    set_pte(page, page_table_levels - 1, PAGE_PRESENT | flags, frame);
    return ENONE;
}

physaddr_t unmap_page(void *page)
{
    for(int level = 0; level < page_table_levels; level++)
    {
        if((get_pte_type(page, level) & PAGE_PRESENT) == 0)
            return EOUTOFBOUNDS;
    }
    physaddr_t frame = get_pte_address(page, page_table_levels - 1);
    set_pte(page, page_table_levels - 1, 0, 0);
    return frame;
}

int page_type(void *page)
{
    for(int level = 0; level < page_table_levels - 1; level++)
    {
        int flags = get_pte_type(page, level);
        if((flags & PAGE_PRESENT) == 0)
            return flags;
    }
    return get_pte_type(page, page_table_levels - 1);
}

physaddr_t physical_address(void *linear_address)
{
    return get_pte_address(linear_address, page_table_levels - 1);
}

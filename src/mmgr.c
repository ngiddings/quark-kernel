#include "mmgr.h"
#include "string.h"
#include "math.h"
#include "platform/paging.h"
#include "types/status.h"
#include "stdio.h"
#include <libmalloc/buddy_alloc.h>
#include <stdint.h>
#include <stdbool.h>

#define AVAIL_LIST_SIZE 20

buddy_descriptor_t page_alloc;

physaddr_t reserve_pages(size_t size)
{
    unsigned long location = buddy_reserve(&page_alloc, size);
    printf("Reserved  >=%08x pages at %08x\n", size, location);
    if(location != NOMEM)
    {
        return location;
    }
    else
    {
        return ENOMEM;
    }
}

size_t free_pages(physaddr_t location)
{
    return buddy_free(&page_alloc, location);
}

physaddr_t reserve_page()
{
    unsigned long loc = buddy_reserve(&page_alloc, page_size);
    printf("Reserved %08x\n", loc);
    if(loc == NOMEM)
    {
        return ENOMEM;
    }
    else
    {
        return loc;
    }
}

int free_page(physaddr_t location)
{
    buddy_free_size(&page_alloc, location, page_size);
    return ENONE;
}

size_t free_page_count()
{
    return page_alloc.free_block_count;
}

void *page_map_base()
{
    return (void*)page_alloc.block_map;
}

void *page_map_end()
{
    return (void*)page_alloc.block_map + page_alloc.block_map_size;
}

error_t initialize_page_map(memory_map_t *map, void *base, size_t memory_size, unsigned long block_size)
{
    static unsigned long avail_list[AVAIL_LIST_SIZE];
    // Round memory_size up to nearest power of 2
    memory_size = 1 << llog2(memory_size);

    page_alloc.avail = avail_list;
    page_alloc.block_map = (buddy_block_t*) base;
    page_alloc.block_size = block_size;
    page_alloc.mmap = NULL;
    page_alloc.offset = 0;

    /* Allocate pages for bitmap */
    int pages_mapped = 0;
    int pages_needed = (buddy_map_size(map, block_size) + page_size - 1) / page_size;
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
            void *page = (void*)page_alloc.block_map + pages_mapped * page_size;
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
 
    if(buddy_alloc_init(&page_alloc, map))
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

error_t map_region(void *page, physaddr_t frame, size_t size, int flags)
{
    if(frame % page_size != 0)
    {
        return EINVALIDARG;
    }
    for(size_t p = 0; p < size; p += page_size)
    {
        set_pte(page + p, page_table_levels - 1, PAGE_PRESENT | flags, frame + p);
    }
    return ENONE;
}

physaddr_t unmap_region(void *page, size_t size)
{
    physaddr_t frame = get_pte_address(page, page_table_levels - 1);
    for(size_t p = 0; p < size; p += page_size)
    {
        set_pte(page + p, page_table_levels - 1, 0, 0);
    }
    return frame;
}

error_t map_page(void *page, physaddr_t frame, int flags)
{
    if (frame % page_size != 0)
    {
        return EINVALIDARG;
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

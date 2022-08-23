#include "mmgr.h"
#include "string.h"
#include "math.h"
#include "platform/paging.h"
#include "types/status.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Describes a stack containing the physical addresses of available page
 * frames.
 * 
 */
struct page_stack_t
{
    
    /**
     * @brief The total number of physical pages managed by the system.
     * 
     */
    unsigned long total_pages;

    /**
     * @brief Points to the topmost physical address on the stack.
     * 
     */
    physaddr_t *stack_pointer;

    /**
     * @brief Points to the bottom of the stack.
     * 
     */
    physaddr_t *base_pointer;

    /**
     * @brief Points to the limit of the stack. The stack cannot grow beyond
     * this point.
     * 
     */
    physaddr_t *limit_pointer;

} page_stack;

struct page_map_t
{
    /**
     * @brief The underlying bitmap representing the availability of chunks of
     * physical memory.
     * 
     */
    unsigned long *bitmap;

    /**
     * @brief The size of the bitmap in bytes.
     * 
     */
    unsigned long bitmap_size;

    /**
     * @brief The size in bytes of the smallest unit of allocation.
     * 
     * This value should either be the size of a page on the host system, or
     * possibly some number of pages.
     * 
     */
    unsigned long block_size;

    /**
     * @brief 
     * 
     */
    unsigned long height;

    /**
     * @brief The number of available blocks of memory. 
     * 
     * Due to memory fragmentation, it may not be possible to allocate all 
     * available memory at once.
     * 
     */
    unsigned long free_block_count;
} page_map;

const int bitmap_word_size = 8 * sizeof(*page_map.bitmap);

int split_block(int index)
{
    if(index)
    {
        int bitmap_index = index / bitmap_word_size;
        int bitmap_offset = index % bitmap_word_size;
        page_map.bitmap[bitmap_index] &= ~(1 << bitmap_offset);
        index *= 2;
        bitmap_index = index / bitmap_word_size;
        bitmap_offset = index / bitmap_word_size;
        page_map.bitmap[bitmap_index] |= 1 << bitmap_offset;
        page_map.bitmap[bitmap_index] |= 1 << (bitmap_offset ^ 1);
    }
    return index;
}

int find_free_region(int height)
{
    if(height > page_map.height || height < 0)
    {
        return 0;
    }
    else if(height <= page_map.height - ilog2(bitmap_word_size))
    {
        for(int index = (1 << (page_map.height - height)) / bitmap_word_size; 
            index < (1 << (page_map.height - height + 1)) / bitmap_word_size; 
            index++)
        {
            if(page_map.bitmap[index] != 0)
            {
                return bitmap_word_size * index + __builtin_ctz(page_map.bitmap[index]);
            }
        }
    }
    else
    {
        static const int bitmasks[] = {0x00000002, 0x0000000C, 0x000000F0, 0x0000FF00, 0xFFFF0000};
        int depth = page_map.height - height;
        if(page_map.bitmap[0] & bitmasks[depth])
        {
            return __builtin_ctz(page_map.bitmap[0] & bitmasks[depth]);
        }
    }
    return split_block(find_free_region(height + 1));
}

physaddr_t reserve_region(size_t size)
{
    int height = llog2(size / page_map.block_size);
    int index = find_free_region(height);
    if(index)
    {
        int bitmap_index = index / bitmap_word_size;
        int bitmap_offset = index % bitmap_word_size;
        page_map.bitmap[bitmap_index] &= ~(1 << bitmap_offset);
        return (page_map.block_size << height) * (index - (1 << (page_map.height - height)));
    }
    else
    {
        return ENOMEM;
    }
}

int free_region(physaddr_t location, size_t size)
{
    int height = llog2(size / page_map.block_size);
    int index = (location / (page_map.block_size * (1 << height))) + (1 << (page_map.height - height));
    int bitmap_index = index / bitmap_word_size;
    int bitmap_offset = index % bitmap_word_size;
    page_map.bitmap[bitmap_index] |= 1 << bitmap_offset;
    while(page_map.bitmap[bitmap_index] & (1 << (bitmap_offset ^ 1)))
    {
        page_map.bitmap[bitmap_index] &= ~(1 << bitmap_offset);
        page_map.bitmap[bitmap_index] &= ~(1 << (bitmap_offset ^ 1));
        index /= 2;
        bitmap_index = index / bitmap_word_size;
        bitmap_offset = index % bitmap_word_size;
        page_map.bitmap[bitmap_index] |= 1 << bitmap_offset;
    }
    return ENONE;
}

physaddr_t reserve_page()
{
    return reserve_region(page_size);
}

int free_page(physaddr_t location)
{
    return free_region(location, page_size);
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

enum error_t initialize_page_map(struct memory_map_t *map, void *base, size_t memory_size, unsigned long block_size)
{
    // Round memory_size up to nearest power of 2
    memory_size = 1 << llog2(memory_size);
    page_map.bitmap = (unsigned long*) base;
    page_map.bitmap_size = (memory_size / page_size) / 4;
    page_map.block_size = block_size;
    page_map.height = llog2(memory_size / block_size);
    page_map.free_block_count = 0;
    int block_log = llog2(block_size);
    int pages_mapped = 0;
    for(int i = 0; i < map->size; i++)
    {
        if(map->array[i].type != M_AVAILABLE)
        {
            continue;
        }
        physaddr_t location = (map->array[i].location + page_size - 1) & ~(page_size - 1);
        physaddr_t region_end = map->array[i].location + map->array[i].size;
        while(location + block_size <= region_end)
        {
            if(pages_mapped < page_map.bitmap_size / page_size)
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
                location += page_size;
                continue;
            }
            int bit_offset = (location / block_size) % bitmap_word_size;
            int bitmap_index = (location / block_size) / bitmap_word_size;
            size_t chunk_size = (bitmap_word_size - bit_offset) * block_size;
            if(bit_offset == 0 && (region_end - location) >= chunk_size)
            {
                // Set all bits in the word
                page_map.bitmap[bitmap_index] = ~0;
            }
            else if(bit_offset == 0)
            {
                // Set the first 'count' bits
                int count = (region_end - location) >> block_log;
                page_map.bitmap[bitmap_index] |= (1 << count) - 1;
            }
            else if((region_end - location) >= chunk_size)
            {
                // Set all bits starting at 'bit_offset'
                page_map.bitmap[bitmap_index] |= ~((1 << bit_offset) - 1);
            }
            else
            {
                // Set all bits starting at 'bit_offset' up to 'count'
                int count = (region_end - location) >> block_log;
                page_map.bitmap[bitmap_index] |= ((1 << count) - 1) & ~((1 << bit_offset) - 1);
            }
            location += chunk_size;
        }
    }
    return ENONE;
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

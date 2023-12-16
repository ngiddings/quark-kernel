#pragma once

#include "sharedobject.h"
#include "platform/paging.h"
#include "types/physaddr.h"
#include "types/status.h"
#include "libmalloc/memmap.h"
#include <stddef.h>

extern const size_t page_size;

/**
 * @brief 
 * 
 * @param size 
 * @return physaddr_t 
 */
physaddr_t reserve_pages(size_t size);

/**
 * @brief 
 * 
 * @param location 
 * @param size 
 * @return int 
 */
size_t free_pages(physaddr_t location);

/**
 * @brief Reserves a single page and returns its physical address.
 * 
 * @param stack 
 * @return physaddr_t 
 */
physaddr_t reserve_page();

/**
 * @brief Marks the page at the given location as free.
 * 
 * @param stack 
 * @param location 
 */
int free_page(physaddr_t location);

/**
 * @brief Computes the number of available pages.
 * 
 * @param stack 
 * @return size_t 
 */
size_t free_page_count();

/**
 * @brief Get the location of the bottom of the page stack.
 * 
 * @return void* 
 */
void *page_map_base();

/**
 * @brief Get the location of the top of the page stack.
 * 
 * @return void* 
 */
void *page_map_end();

/**
 * @brief Constructs the bitmaps used by the physical memory allocator.
 * 
 * @param map 
 * @param base 
 * @param memory_size 
 * @param block_size 
 * @return enum error_t 
 */
error_t initialize_page_map(memory_map_t *map, void *base, size_t memory_size, unsigned long block_size);

/**
 * @brief Create a new top-level page table and map the kernel in it.
 * 
 * This function does not load the page table; it only initializes it.
 * 
 * @return physaddr_t 
 */
physaddr_t create_address_space();

/**
 * @brief Returns the physical address of the top-level page table currently in
 * use.
 * 
 * @param table 
 * @return physaddr_t 
 */
physaddr_t current_address_space();

/**
 * @brief Maps a region in virtual memory with the specified flags.
 * 
 * @param page
 * @param frame
 * @param size
 * @param flags
 */
error_t map_region(void *page, physaddr_t frame, size_t size, int flags);

/**
 * @brief Maps a shared object into virtual memory.
 * 
 * @param location 
 * @param object 
 * @return 
 */
error_t map_object(void *location, shared_object_t *object);

/**
 * @brief Unmaps a region in virtual memory. All pages which contain some part
 * of the region specified will be unmapped.
 * 
 * @param page
 * @param size
 * 
 * @returns The physical address of the first page that was unmapped
 */
physaddr_t unmap_region(void *page, size_t size);

/**
 * @brief Maps a single page with the specified flags.
 * 
 * @param page 
 * @param frame 
 * @param flags 
 * @return int 
 */
error_t map_page(void *page, physaddr_t frame, int flags);

/**
 * @brief Unmaps a single page, returning the physical address of the frame it
 * pointed to.
 * 
 * @param page 
 * @return physaddr_t 
 */
physaddr_t unmap_page(void *page);

/**
 * @brief 
 * 
 * @param page 
 * @return enum page_type_t 
 */
int page_type(void *page);

/**
 * @brief 
 * 
 * @param linear_address 
 * @return physaddr_t 
 */
physaddr_t physical_address(void *linear_address);

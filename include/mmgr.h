#pragma once

#include "pageallocator.h"
#include "types/physaddr.h"

enum page_flag_t
{
    PAGE_RW = 1,
    PAGE_EXECUTABLE = 1 << 1,
    PAGE_USERMODE = 1 << 2
};

enum page_type_t
{
    PAGE_NOT_PRESENT = 0,
    PAGE_ANON = 1,
    PAGE_COPY_ON_WRITE = 2
};

extern const size_t page_size;

/**
 * @brief Create a new top-level page table and map the kernel in it.
 * 
 * This function does not load the page table; it only initializes it.
 * 
 * @return physaddr_t 
 */
physaddr_t create_address_space(struct page_stack_t *page_stack);

/**
 * @brief Load an existing top-level page table
 * 
 * @param table 
 */
void load_address_space(physaddr_t table);

/**
 * @brief Returns the physical address of the top-level page table currently in
 * use.
 * 
 * @param table 
 * @return physaddr_t 
 */
physaddr_t current_address_space();

/**
 * @brief Maps a single page with the specified flags.
 * 
 * @param page 
 * @param frame 
 * @param flags 
 * @return int 
 */
int map_page(struct page_stack_t *page_stack, void *page, physaddr_t frame, int flags);

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
enum page_type_t page_type(void *page);

#pragma once

#include "memmap.h"
#include "platform/paging.h"
#include "types/physaddr.h"
#include <stddef.h>

extern const size_t page_size;

/**
 * @brief Pop the topmost address from the stack and returns that value.
 * 
 * If the stack is empty, this function will instead return a status code. This
 * can be identified by testing the least signifigant bits of the return value.
 * 
 * @param stack 
 * @return physaddr_t 
 */
physaddr_t reserve_page();

/**
 * @brief Pushes `location` onto the stack.
 * 
 * If there is no room on the stack, the stack will be unaffected.
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
void *page_stack_bottom();

/**
 * @brief Get the location of the top of the page stack.
 * 
 * @return void* 
 */
void *page_stack_top();

/**
 * @brief Push all available pages in `map` onto the stack
 * 
 * @param stack 
 * @param map 
 */
int initialize_page_stack(struct memory_map_t *map, physaddr_t *stack_base);

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
 * @brief Maps a single page with the specified flags.
 * 
 * @param page 
 * @param frame 
 * @param flags 
 * @return int 
 */
int map_page(void *page, physaddr_t frame, int flags);

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

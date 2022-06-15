#pragma once

#include "types/physaddr.h"
#include <stddef.h>

enum page_flag_t
{
    PAGE_RW = 1,
    PAGE_EXECUTABLE = 1 << 1,
    PAGE_USERMODE = 1 << 2,
    PAGE_PRESENT = 1 << 16,
    PAGE_ANON = 1 << 17,
    PAGE_COPY_ON_WRITE = 1 << 18
};

const size_t page_table_levels;

/**
 * @brief 
 * 
 * @param table 
 * @return int 
 */
int paging_init_top_table(physaddr_t table);

/**
 * @brief Returns the physical address of the top-level page table currently in
 * use.
 * 
 * @return physaddr_t 
 */
physaddr_t paging_current_address_space();

/**
 * @brief Load an existing top-level page table
 * 
 * @param table 
 */
void paging_load_address_space(physaddr_t table);

/**
 * @brief Get the pte type object
 * 
 * @param page 
 * @param level 
 * @return int 
 */
int get_pte_type(void *page, int level);

/**
 * @brief Set the pte type object
 * 
 * @param page 
 * @param level 
 * @param flags 
 * @return int 
 */
int set_pte_type(void *page, int level, int flags);

/**
 * @brief Get the pte address object
 * 
 * @param page 
 * @param level 
 * @return physaddr_t 
 */
physaddr_t get_pte_address(void *page, int level);

/**
 * @brief Set the pte address object
 * 
 * @param page 
 * @param level 
 * @param addr 
 * @return int 
 */
int set_pte_address(void *page, int level, physaddr_t addr);

/**
 * @brief Set the pte object
 * 
 * @param page 
 * @param level 
 * @param flags 
 * @param addr 
 * @return int 
 */
int set_pte(void *page, int level, int flags, physaddr_t addr);

/**
 * @brief Resets all entries in the same table as the specified entry
 * 
 * @param page 
 * @param level 
 */
void wipe_page_table(void *page, int level);
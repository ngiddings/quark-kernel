#pragma once

#include "types/physaddr.h"
#include <stddef.h>

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
    size_t total_pages;

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
};

/**
 * @brief Pop the topmost address from the stack and returns that value.
 * 
 * If the stack is empty, this function will instead return a status code. This
 * can be identified by testing the least signifigant bits of the return value.
 * 
 * @param stack 
 * @return physaddr_t 
 */
physaddr_t reserve_page(struct page_stack_t *stack);

/**
 * @brief Pushes `location` onto the stack.
 * 
 * If there is no room on the stack, the stack will be unaffected.
 * 
 * @param stack 
 * @param location 
 */
int free_page(struct page_stack_t *stack, physaddr_t location);
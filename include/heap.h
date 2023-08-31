#pragma once

#include <stddef.h>

/**
 * @brief Initializes the system's heap.
 * 
 * This function contructs the heap's internal tables, allocating pages as needed
 * to do so.
 * 
 * @param base Base location of the heap to contruct
 * @param heap_size Total size in bytes of the heap
 * @return a status code
 */
int kminit(void *base, size_t heap_size);

/**
 * @brief Allocates a block of memory containing at least `size` bytes.
 * 
 * @param size The size of the block to allocate
 * @return void* A pointer to the allocated block, or NULL upon failure.
 */
void *kmalloc(size_t size);

/**
 * @brief Frees a block of memory previously allocated by `kmalloc`.
 * 
 * @param ptr Pointer to the block to free
 */
void kfree(void *ptr);

#pragma once

#include "process.h"
#include <stddef.h>

/**
 * @brief 
 * 
 */
struct priority_queue_t
{
    /**
     * @brief A pointer to the heap described by this structure.
     * 
     */
    struct process_t **heap;

    /**
     * @brief The current number of elements stored in the heap.
     * 
     */
    size_t size;

    /**
     * @brief The maximum number of elements that the heap can currently hold.
     * 
     */
    size_t capacity;
};

/**
 * @brief 
 * 
 * @param queue 
 * @return struct process_t* 
 */
struct process_t *extract_min(struct priority_queue_t *queue);

/**
 * @brief 
 * 
 * @param queue 
 * @param process 
 * @return int 
 */
int insert(struct priority_queue_t *queue, struct process_t *process);

/**
 * @brief 
 * 
 * @param queue 
 * @param process 
 * @return int 
 */
int remove(struct priority_queue_t *queue, struct process_t *process);
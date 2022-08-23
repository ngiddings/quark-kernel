#pragma once

#include "types/status.h"

struct queue_node_t;

/**
 * @brief A last-in-first-out queue.
 * 
 */
struct queue_t
{
    /**
     * @brief The number of objects stored in this queue.
     * 
     */
    unsigned int count;

    /**
     * @brief A link to the first node stored in this queue.
     * 
     */
    struct queue_node_t *first;

    /**
     * @brief A link to the last node stored in this queue.
     * 
     */
    struct queue_node_t *last;
};

/**
 * @brief Initializes a queue structure.
 * 
 * @param queue 
 */
void queue_construct(struct queue_t *queue);

/**
 * @brief Inserts a new item at the end of the queue.
 * 
 * @param queue 
 * @param ptr A pointer to some user-defined data to store on the queue.
 * @return enum error_t
 */
enum error_t queue_insert(struct queue_t *queue, void *ptr);

/**
 * @brief Removes the next item from the queue and returns it.
 * 
 * @param queue 
 * @return void* The pointer stored at the front of the queue.
 */
void *queue_get_next(struct queue_t *queue);

/**
 * @brief Returns the next item on the queue without removing it.
 * 
 * @param queue 
 * @return void* The pointer stored at the front of the queue.
 */
void *queue_peek(struct queue_t *queue);


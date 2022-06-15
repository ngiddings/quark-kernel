#pragma once

/**
 * @brief An pair consisting of a priority and a pointer to be stored in 
 * a priority queue.
 * 
 */
struct priority_queue_node_t
{
    /**
     * @brief A pointer to some user-defined object.
     * 
     */
    void *value;

    /**
     * @brief The priority of the associated object. Only the object with the
     * lowest priority is directly accessible from a priority queue.
     * 
     */
    int priority;
};

/**
 * @brief A priority queue. Each object in the queue contains a priority and
 * a pointer to some user-defined value, and a priority. Operations on the
 * queue consist of insertion and deletion of objects, and extraction of the
 * object with the lowest priority.
 * 
 */
struct priority_queue_t
{
    /**
     * @brief A pointer to the heap described by this structure.
     * 
     */
    struct priority_queue_node_t *heap;

    /**
     * @brief The current number of elements stored in the heap.
     * 
     */
    int size;

    /**
     * @brief The maximum number of elements that the heap can currently hold.
     * 
     */
    int capacity;
};

/**
 * @brief Initializes the given queue struct.
 * 
 * The queue's underlying heap is allocated
 * 
 * @param queue 
 * @return int 
 */
int construct_priority_queue(struct priority_queue_t *queue, int capacity);

/**
 * @brief Extracts the object with the lowest priority off the given queue.
 * 
 * The object is removed from the queue, and a pointer to its userdata is
 * returned. If the queue is empty, this function returns NULL and the queue
 * is unaffected. If multiple objects with the same priority are stored on the
 * queue, this function will extract the object lest-recently inserted.
 * 
 * @param queue 
 * @return void* 
 */
void *extract_min(struct priority_queue_t *queue);

/**
 * @brief Inserts a new object onto the queue.
 * 
 * @param queue 
 * @param value 
 * @param priority
 * @return int 
 */
int queue_insert(struct priority_queue_t *queue, void *value, int priority);

/**
 * @brief Removes the object with a matching value from the queue.
 * 
 * @param queue 
 * @param value 
 * @return int 
 */
int queue_remove(struct priority_queue_t *queue, void *value);

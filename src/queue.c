#include "queue.h"
#include "heap.h"

struct queue_node_t
{
    /**
     * @brief Pointer to the user-defined object this node refers to.
     * 
     */
    void *ptr;

    /**
     * @brief Link to the next node in the queue.
     * 
     */
    struct queue_node_t *next;
};

void queue_construct(struct queue_t *queue)
{
    queue->first = NULL;
    queue->last = NULL;
    queue->count = 0;
}

void queue_insert(struct queue_t *queue, void *ptr)
{
    struct queue_node_t *node = kmalloc(sizeof(struct queue_node_t));
    node->ptr = ptr;
    node->next = NULL;
    if(queue->last == NULL)
    {
        queue->first = queue->last = node;
    }
    else
    {
        queue->last->next = node;
        queue->last = node;
    }
    queue->count++;
}

void *queue_get_next(struct queue_t *queue)
{
    struct queue_node_t *node = queue->first;
    queue->first = node->next;
    if(queue->first == NULL)
    {
        queue->last = NULL;
    }
    queue->count--;
    void *ptr = node->ptr;
    kfree(node);
    return ptr;
}

void *queue_peek(struct queue_t *queue)
{
    return queue->first->ptr;
}
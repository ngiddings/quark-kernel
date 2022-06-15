#include "priorityqueue.h"
#include "heap.h"
#include "types/status.h"

void heapify(struct priority_queue_t *queue, size_t i)
{
    if(i * 2 + 1 >= queue->size)
    {
        return;
    }
    else if(queue->heap[i * 2 + 1].priority <= queue->heap[i].priority
        && queue->heap[i * 2 + 1].priority <= queue->heap[i * 2 + 2].priority)
    {
        struct priority_queue_node_t buffer = queue->heap[i];
        queue->heap[i] = queue->heap[i * 2 + 1];
        queue->heap[i * 2 + 1] = buffer;
        heapify(queue, i * 2 + 1);
    }
    else if(queue->heap[i * 2 + 2].priority <= queue->heap[i].priority
        && queue->heap[i * 2 + 2].priority <= queue->heap[i * 2 + 1].priority)
    {
        struct priority_queue_node_t buffer = queue->heap[i];
        queue->heap[i] = queue->heap[i * 2 + 2];
        queue->heap[i * 2 + 2] = buffer;
        heapify(queue, i * 2 + 2);
    }
}

int construct_priority_queue(struct priority_queue_t *queue, int capacity)
{
    queue->heap = kmalloc(sizeof(struct priority_queue_node_t) * capacity);
    if(queue->heap == NULL)
    {
        return S_OUT_OF_MEMORY;
    }
    queue->capacity = capacity;
    queue->size = 0;
    return S_OK;
}

void *extract_min(struct priority_queue_t *queue)
{
    if(queue->size == 0)
        return NULL;
    queue->size--;
    void *value = queue->heap[0].value;
    queue->heap[0] = queue->heap[queue->size];
    heapify(queue, 0);
    return value;
}

int queue_insert(struct priority_queue_t *queue, void *value, int priority)
{
    if(queue->size == queue->capacity)
        return S_OUT_OF_MEMORY;
    size_t i = queue->size;
    queue->size++;
    while(i > 0 && queue->heap[(i - 1) / 2].priority > priority)
    {
        queue->heap[i] = queue->heap[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    queue->heap[i].priority = priority;
    queue->heap[i].value = value;
    return S_OK;
}

int queue_remove(struct priority_queue_t *queue, void *value)
{
    for(size_t i = 0; i < queue->size; i++)
    {
        if(queue->heap[i].value == value)
        {
            queue->size--;
            queue->heap[i] = queue->heap[queue->size];
            heapify(queue, i);
            return S_OK;
        }
    }
    return S_OUT_OF_BOUNDS;
}
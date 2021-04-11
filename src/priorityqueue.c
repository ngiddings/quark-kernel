#include "priorityqueue.h"
#include "types/status.h"

void heapify(struct priority_queue_t *queue, size_t i)
{
    if(i * 2 + 1 >= queue->size)
    {
        return;
    }
    else if(queue->heap[i * 2 + 1]->priority <= queue->heap[i]->priority
        && queue->heap[i * 2 + 1]->priority <= queue->heap[i * 2 + 2]->priority)
    {
        struct process_t *buffer = queue->heap[i];
        queue->heap[i] = queue->heap[i * 2 + 1];
        queue->heap[i * 2 + 1] = buffer;
        heapify(queue, i * 2 + 1);
    }
    else if(queue->heap[i * 2 + 2]->priority <= queue->heap[i]->priority
        && queue->heap[i * 2 + 2]->priority <= queue->heap[i * 2 + 1]->priority)
    {
        struct process_t *buffer = queue->heap[i];
        queue->heap[i] = queue->heap[i * 2 + 2];
        queue->heap[i * 2 + 2] = buffer;
        heapify(queue, i * 2 + 2);
    }
}

struct process_t *extract_min(struct priority_queue_t *queue)
{
    if(queue->size == 0)
        return NULL;
    queue->size--;
    struct process_t *p = queue->heap[0];
    queue->heap[0] = queue->heap[queue->size];
    heapify(queue, 0);
    return p;
}

int insert(struct priority_queue_t *queue, struct process_t *process)
{
    if(queue->size == queue->capacity)
        return S_OUT_OF_MEMORY;
    size_t i = queue->size;
    queue->size++;
    while(i > 0 && queue->heap[(i - 1) / 2]->priority > process->priority)
    {
        queue->heap[i] = queue->heap[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    queue->heap[i] = process;
    return S_OK;
}

int remove(struct priority_queue_t *queue, struct process_t *process)
{
    for(size_t i = 0; i < queue->size; i++)
    {
        if(queue->heap[i] == process)
        {
            queue->size--;
            queue->heap[i] = queue->heap[queue->size];
            heapify(queue, i);
            return S_OK;
        }
    }
    return S_OUT_OF_BOUNDS;
}
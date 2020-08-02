#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>

namespace kernel
{

class Process
{
public:

    Process();

    size_t priority;

    void* stack;

};

class ProcessQueue
{
public:

    ProcessQueue();

    ProcessQueue(Process** array);

    Process* extractMin();

    void insert(Process* n);

    void remove(Process* n);

private:

    void heapify(size_t index);

    Process** m_array;

    size_t m_size;

};

};

#endif
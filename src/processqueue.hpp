#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>

#include "process.hpp"

namespace kernel
{

class ProcessQueue
{
public:

    ProcessQueue(Process** array);

    Process* extractMin();

    void insert(Process* n);

    void remove(Process* n);

private:

    void heapify(size_t index);

    Process** m_array;

    size_t m_size;

};

}

#endif
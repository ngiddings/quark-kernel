#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>

#include "process.hpp"

namespace kernelns
{

template<class T>
class Heap
{
public:

    Heap();

    Heap(T** array, size_t maxSize);

    T* extractMin();

    void insert(T* n);

    void remove(T* n);

private:

    void heapify(size_t index);

    T** m_array;

    size_t m_size, m_limit;

};

};

#endif
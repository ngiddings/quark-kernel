#include "heap.hpp"

template<class T>
kernelns::Heap<T>::Heap()
{
    
}

template<class T>
kernelns::Heap<T>::Heap(T** array, size_t maxSize)
{
    m_array = array;
    m_size = 0;
    m_limit = maxSize;
}

template<class T>
T* kernelns::Heap<T>::extractMin()
{
    if(m_size == 0)
        return nullptr;
    m_size--;
    T* p = m_array[0];
    m_array[0] = m_array[m_size];
    heapify(0);
    return p;
}

template<class T>
void kernelns::Heap<T>::insert(T* n)
{
    if(m_size == m_limit) // TODO: have insert return status code
        return;
    size_t i = m_size;
    m_size++;
    while(i > 0 && *m_array[(i - 1) / 2] > *n)
    {
        m_array[i] = m_array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    m_array[i] = n;
}

template<class T>
void kernelns::Heap<T>::remove(T* n)
{
    for(size_t i = 0; i < m_size; i++)
    {
        if(m_array[i] == n)
        {
            m_size--;
            m_array[i] = m_array[m_size];
            heapify(i);
            break;
        }
    }
}

template<class T>
void kernelns::Heap<T>::heapify(size_t i)
{
    if(i * 2 + 1 >= m_size)
		return;
    if (*m_array[i * 2 + 1] <= *m_array[i] && *m_array[i * 2 + 1] <= *m_array[i * 2 + 2])
    {
        T buffer = m_array[i];
        m_array[i] = m_array[i * 2 + 1];
        m_array[i * 2 + 1] = buffer;
        heapify(i * 2 + 1);
    }
    else if (*m_array[i * 2 + 2] <= *m_array[i] && *m_array[i * 2 + 2] <= *m_array[i * 2 + 1])
    {
        T buffer = m_array[i];
        m_array[i] = m_array[i * 2 + 2];
        m_array[i * 2 + 2] = buffer;
        heapify(i * 2 + 2);
    }
}
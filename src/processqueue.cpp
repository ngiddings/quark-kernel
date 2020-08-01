#include "processqueue.hpp"

kernel::ProcessQueue::ProcessQueue(Process** array)
{
    m_array = array;
    m_size = 0;
}

kernel::Process* kernel::ProcessQueue::extractMin()
{
    if(m_size == 0)
        return NULL;
    m_size--;
    Process* p = m_array[0];
    m_array[0] = m_array[m_size];
    heapify(0);
}

void kernel::ProcessQueue::insert(Process* n)
{
    size_t i = m_size;
    m_size++;
    for(; i > 0 && m_array[(i - 1) / 2]->priority > n->priority; i = (i - 1) / 2)
	{
		m_array[i] = m_array[(i - 1) / 2];
	}
    m_array[i] = n;
}

void kernel::ProcessQueue::remove(Process* n)
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

void kernel::ProcessQueue::heapify(size_t i)
{
    if(i * 2 + 1 >= m_size)
			return;
    if (m_array[i]->priority >= m_array[i * 2 + 1]->priority && m_array[i * 2 + 1]->priority <= m_array[i * 2 + 2]->priority)
    {
        Process* buffer = m_array[i];
        m_array[i] = m_array[i * 2 + 1];
        m_array[i * 2 + 1] = buffer;
        heapify(i * 2 + 1);
    }
    else if (m_array[i]->priority >= m_array[i * 2 + 2]->priority && m_array[i * 2 + 2]->priority <= m_array[i * 2 + 1]->priority)
    {
        Process* buffer = m_array[i];
        m_array[i] = m_array[i * 2 + 2];
        m_array[i * 2 + 2] = buffer;
        heapify(i * 2 + 2);
    }
}
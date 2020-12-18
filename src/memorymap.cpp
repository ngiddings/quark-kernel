#include "memorymap.hpp"

using namespace kernelns;

MemoryMap::MemoryMap()
{
	this->m_entries = 0;
}

MemoryMap::MemoryMap(MemoryMap& copy)
{
	for(int i = 0; i < copy.m_entries; i++)
		m_map[i] = copy[i];
	this->m_entries = copy.m_entries;
}

const MemoryRegion& MemoryMap::operator[](size_t index) const
{
	return m_map[index];
}

size_t MemoryMap::size() const
{
	return m_entries;
}

size_t MemoryMap::totalMemory() const
{
	size_t sum = 0;
	for(int i = 0; i < m_entries; i++)
	{
		sum += m_map[i].getSize();
	}
	return sum;
}

size_t MemoryMap::totalMemory(unsigned int type) const
{
	size_t sum = 0;
	for(int i = 0; i < m_entries; i++)
	{
		if(m_map[i].getType() == type)
			sum += m_map[i].getSize();
	}
	return sum;
}

bool MemoryMap::overlaps(const MemoryRegion& region) const
{
	for(int i = 0; i < m_entries; i++)
	{
		if(m_map[i].overlaps(region))
			return true;
	}
	return false;
}

bool MemoryMap::contains(const MemoryRegion& region) const
{
	for(int i = 0; i < m_entries; i++)
	{
		if(m_map[i].contains(region) && m_map[i].getType() == region.getType())
			return true;
	}
	return false;
}

void MemoryMap::insertEntry(const MemoryRegion& region)
{
	simpleInsert(region);
	unsigned int i = 0;
	while(i != invalidIndex)
		i = trim(i);
}

void MemoryMap::insertEntry(physaddr_t location, size_t size, unsigned int type)
{
    insertEntry(MemoryRegion(location, size, type));
}

void MemoryMap::remove(unsigned int index)
{
	for(unsigned int i = index; i < (m_entries - 1); i++)
	{
		m_map[i] = m_map[i+1];
	}
	m_entries--;
}

void MemoryMap::simpleInsert(const MemoryRegion& region)
{
	MemoryRegion newRegion(region);
	unsigned int i = 0;
    while(i < m_entries)
    {
        if(newRegion < m_map[i])
		{
			MemoryRegion buffer = newRegion;
			newRegion = m_map[i];
			m_map[i] = buffer;
		}
		i++;
    }
	m_map[i] = newRegion;
	m_entries++;
}

void MemoryMap::simpleInsert(physaddr_t location, size_t size, unsigned int type)
{
	simpleInsert(MemoryRegion(location, size, type));
}

unsigned int MemoryMap::trim(unsigned int index)
{
	if(index + 1 >= m_entries)
		return invalidIndex;
	MemoryRegion& left = m_map[index];
	MemoryRegion& right = m_map[index+1];
	if(left.overlaps(right))
	{
		if(left.getType() == right.getType())
		{
			left = MemoryRegion(left.getLocation(), 
				(right.getEnd() > left.getEnd() ? right.getEnd() : left.getEnd()) - left.getLocation(),
				left.getType());
			remove(index + 1);
			return index;
		}
		else if(left.getType() < right.getType())
		{
			if(right.contains(left))
			{
				remove(index);
				return index;
			}
			else if(left.getEnd() <= right.getEnd())
			{
				left.truncateRight(right.getLocation());
				return index + 1;
			}
			else
			{
				MemoryRegion newRight(right.getEnd(), left.getEnd() - right.getEnd(), left.getType());
				left.truncateRight(right.getLocation());
				if(left.getSize() == 0)
					remove(index);
				simpleInsert(newRight);
				return index + 2;
			}
		}
		else
		{
			if(left.contains(right))
			{
				remove(index + 1);
				return index;
			}
			else
			{
				right.truncateLeft(left.getEnd());
				return index + 1;
			}
		}
	}
	else if(left.bordersLeft(right) && left.getType() == right.getType())
	{
		left = MemoryRegion(left.getLocation(), right.getEnd() - left.getLocation(), left.getType());
		remove(index + 1);
		return index;
	}
	return index + 1;
}
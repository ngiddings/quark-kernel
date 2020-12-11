#include "memorymap.hpp"

using namespace kernelns;

MemoryMap::Region::Region()
{
	m_location = 0;
	m_size = 0;
	m_type = (Type) 0;
}

MemoryMap::Region::Region(Region& copy)
{
	m_location = copy.m_location;
	m_size = copy.m_size;
	m_type = copy.m_type;
}

MemoryMap::Region::Region(physaddr_t location, size_t size, Type type)
{
	m_location = location;
	m_size = size;
	m_type = (size_t) type;
}

const MemoryMap::Region& MemoryMap::Region::operator=(const MemoryMap::Region& rhs)
{
	m_location = rhs.m_location;
	m_size = rhs.m_size;
	m_type = rhs.m_type;
	return rhs;
}

const bool MemoryMap::Region::operator==(const MemoryMap::Region& rhs) const
{
    return (m_location == rhs.m_location) && (m_size == rhs.m_size);
}

const bool MemoryMap::Region::operator<(const MemoryMap::Region& rhs) const
{
    return (m_location < rhs.m_location) || ((m_location == rhs.m_location) && (m_size < rhs.m_size));
}

const bool MemoryMap::Region::operator>(const MemoryMap::Region& rhs) const
{
    return (m_location > rhs.m_location) || ((m_location == rhs.m_location) && (m_size > rhs.m_size));
}

const bool MemoryMap::Region::operator<=(const MemoryMap::Region& rhs) const
{
    return (m_location < rhs.m_location) || ((m_location == rhs.m_location) && (m_size <= rhs.m_size));
}

const bool MemoryMap::Region::operator>=(const MemoryMap::Region& rhs) const
{
    return (m_location > rhs.m_location) || ((m_location == rhs.m_location) && (m_size >= rhs.m_size));
}

physaddr_t MemoryMap::Region::getLocation() const
{
	return m_location;
}

size_t MemoryMap::Region::getSize() const
{
	return m_size;
}

MemoryMap::Type MemoryMap::Region::getType() const
{
    return (Type) m_type;
}

physaddr_t MemoryMap::Region::getEnd() const
{
	return m_location + m_size;
}

bool MemoryMap::Region::contains(const MemoryMap::Region& r) const
{
	return contains(r.m_location, r.m_size);
}

bool MemoryMap::Region::contains(physaddr_t location, size_t size) const
{
	return (location >= m_location) &&
		(location + size <= m_location + m_size);
}

bool MemoryMap::Region::overlaps(const MemoryMap::Region& r) const
{
	if(r.m_location < m_location)
    {
        return r.m_location + r.m_size < m_location;
    }
    else
    {
        return r.m_location >= m_location && r.m_location < m_location + m_size;
    }
}

bool MemoryMap::Region::bordersLeft(const MemoryMap::Region& right) const
{
    return m_location + m_size == right.m_location;
}

bool MemoryMap::Region::bordersRight(const MemoryMap::Region& left) const
{
    return m_location == left.m_location + left.m_size;
}

bool MemoryMap::Region::borders(const MemoryMap::Region& r) const
{
    return bordersLeft(r) || bordersRight(r);
}

void MemoryMap::Region::truncateLeft(physaddr_t left)
{
	m_size = getEnd() - left;
	m_location = left;
}

void MemoryMap::Region::truncateRight(physaddr_t right)
{
	m_size = right - m_location;
}

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

const MemoryMap::Region& MemoryMap::operator[](size_t index) const
{
	return m_map[index];
}

size_t MemoryMap::size() const
{
	return m_entries;
}

void MemoryMap::insertEntry(physaddr_t location, size_t size, Type type)
{
    Region newRegion(location, size, type);
	unsigned int i = 0;
    while(i < m_entries)
    {
        if(newRegion < m_map[i])
		{
			Region buffer = newRegion;
			newRegion = m_map[i];
			m_map[i] = buffer;
		}
		i++;
    }
	m_map[i] = newRegion;
	m_entries++;
	for(i = 0; i < (m_entries - 1); i++)
	{
		if(m_map[i].overlaps(m_map[i+1]))
		{
			if(m_map[i].getType() == m_map[i+1].getType())
			{
				m_map[i] = Region(m_map[i].getLocation(), 
					(m_map[i+1].getEnd() > m_map[i].getEnd() ? m_map[i+1].getEnd() : m_map[i].getEnd()) - m_map[i].getLocation(),
					m_map[i].getType());
				remove(i + 1);
				i--;
			}
			else if(m_map[i].getType() == AVAILABLE)
			{
				if(m_map[i+1].contains(m_map[i]))
				{
					remove(i);
					i--;
				}
				else if(m_map[i].getEnd() <= m_map[i+1].getEnd())
				{
					m_map[i].truncateRight(m_map[i+1].getLocation());
				}
				else
				{
					unsigned int end = m_map[i].getEnd();
					m_map[i].truncateRight(m_map[i+1].getLocation());
					insertEntry(m_map[i+1].getEnd(), end - m_map[i+1].getEnd(), AVAILABLE);
				}
			}
			else if(m_map[i+1].getType() == AVAILABLE)
			{
				if(m_map[i].contains(m_map[i + 1]))
				{
					remove(i + 1);
					i--;
				}
				else
				{
					m_map[i+1].truncateLeft(m_map[i].getEnd());
				}
			}
			else
			{
				// two overlapping unavailable regions	
			}
		}
		else if(m_map[i].bordersLeft(m_map[i+1]) && m_map[i].getType() == m_map[i+1].getType())
		{
			m_map[i] = Region(m_map[i].getLocation(), m_map[i+1].getEnd() - m_map[i].getLocation(), m_map[i].getType());
			remove(i + 1);
			i--;
		}
	}
}

void MemoryMap::remove(unsigned int index)
{
	for(unsigned int i = index; i < (m_entries - 1); i++)
	{
		m_map[i] = m_map[i+1];
	}
	m_entries--;
}
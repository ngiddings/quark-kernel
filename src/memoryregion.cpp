#include "memoryregion.hpp"

MemoryRegion::MemoryRegion()
{
	m_location = 0;
	m_size = 0;
	m_type = 0;
}

MemoryRegion::MemoryRegion(const MemoryRegion& copy)
{
	m_location = copy.m_location;
	m_size = copy.m_size;
	m_type = copy.m_type;
}

MemoryRegion::MemoryRegion(physaddr_t location, size_t size, unsigned int type)
{
	m_location = location;
	m_size = size;
	m_type = (size_t) type;
}

const MemoryRegion& MemoryRegion::operator=(const MemoryRegion& rhs)
{
	m_location = rhs.m_location;
	m_size = rhs.m_size;
	m_type = rhs.m_type;
	return rhs;
}

bool MemoryRegion::operator==(const MemoryRegion& rhs) const
{
    return (m_location == rhs.m_location) && (m_size == rhs.m_size);
}

bool MemoryRegion::operator!=(const MemoryRegion& rhs) const
{
    return (m_location != rhs.m_location) || (m_size != rhs.m_size);
}

bool MemoryRegion::operator<(const MemoryRegion& rhs) const
{
    return (m_location < rhs.m_location) || ((m_location == rhs.m_location) && (m_size < rhs.m_size));
}

bool MemoryRegion::operator>(const MemoryRegion& rhs) const
{
    return (m_location > rhs.m_location) || ((m_location == rhs.m_location) && (m_size > rhs.m_size));
}

bool MemoryRegion::operator<=(const MemoryRegion& rhs) const
{
    return (m_location < rhs.m_location) || ((m_location == rhs.m_location) && (m_size <= rhs.m_size));
}

bool MemoryRegion::operator>=(const MemoryRegion& rhs) const
{
    return (m_location > rhs.m_location) || ((m_location == rhs.m_location) && (m_size >= rhs.m_size));
}

physaddr_t MemoryRegion::getLocation() const
{
	return m_location;
}

size_t MemoryRegion::getSize() const
{
	return m_size;
}

unsigned int MemoryRegion::getType() const
{
    return m_type;
}

physaddr_t MemoryRegion::getEnd() const
{
	return m_location + m_size;
}

bool MemoryRegion::contains(const MemoryRegion& r) const
{
	return contains(r.m_location, r.m_size);
}

bool MemoryRegion::contains(physaddr_t location, size_t size) const
{
	return (location >= m_location) &&
		(location + size <= m_location + m_size);
}

bool MemoryRegion::overlaps(const MemoryRegion& r) const
{
	if(r.m_location < m_location)
        return r.m_location + r.m_size > m_location;
    else
        return r.m_location < getEnd();
}

bool MemoryRegion::bordersLeft(const MemoryRegion& right) const
{
    return getEnd() == right.m_location;
}

bool MemoryRegion::bordersRight(const MemoryRegion& left) const
{
    return m_location == left.m_location + left.m_size;
}

bool MemoryRegion::borders(const MemoryRegion& r) const
{
    return bordersLeft(r) || bordersRight(r);
}

void MemoryRegion::truncateLeft(physaddr_t left)
{
	m_size = getEnd() - left;
	m_location = left;
}

void MemoryRegion::truncateRight(physaddr_t right)
{
	if(right > m_location)
		m_size = right - m_location;
	else
		m_size = 0;
}
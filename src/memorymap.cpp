#include "memorymap.hpp"

kernel::MemoryMap::MemoryMap(kernel::MemoryMap::Region* map, size_t entries)
{
	this->map = map;
	this->entries = entries;
}

kernel::MemoryMap::Region& kernel::MemoryMap::operator[](size_t index)
{
	return map[index];
}

size_t kernel::MemoryMap::size()
{
	return entries;
}

physaddr_t kernel::MemoryMap::Region::getLocation()
{
	return location;
}

size_t kernel::MemoryMap::Region::getSize()
{
	return size;
}

kernel::MemoryMap::Type kernel::MemoryMap::Region::getType()
{
    return (Type) type;
}

bool kernel::MemoryMap::Region::contains(physaddr_t location, size_t size)
{
	return (location >= this->location) &&
		(location + size <= this->location + this->size);
}

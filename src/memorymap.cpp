#include "memorymap.hpp"

qkernel::MemoryMap::MemoryMap(qkernel::MemoryMap::Region* map, size_t entries)
{
	this->map = map;
	this->entries = entries;
}

qkernel::MemoryMap::Region& qkernel::MemoryMap::operator[](size_t index)
{
	return map[index];
}

size_t qkernel::MemoryMap::size()
{
	return entries;
}

physaddr_t qkernel::MemoryMap::Region::getLocation()
{
	return location;
}

size_t qkernel::MemoryMap::Region::getSize()
{
	return size;
}

qkernel::MemoryMap::Type qkernel::MemoryMap::Region::getType()
{
    return (Type) type;
}

bool qkernel::MemoryMap::Region::contains(physaddr_t location, size_t size)
{
	return (location >= this->location) &&
		(location + size <= this->location + this->size);
}

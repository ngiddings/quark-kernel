#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include <stdint.h>
#include <stddef.h>

#include "systypes.hpp"

namespace qkernel
{

class MemoryMap
{
public:

	enum Type
	{
		AVAILABLE = 1,
		ACPI = 3,
		DEFECTIVE = 5
	};

	class Region
	{
	public:

		physaddr_t getLocation();

		size_t getSize();

		Type getType();

		bool contains(physaddr_t location, size_t size);
	    
	private:

		physaddr_t location;
	    
		size_t size;
	    
		uint32_t type;
	    
	};
	
	MemoryMap(Region* map, size_t entries);

	Region& operator[](size_t index);

	size_t size();

private:

	Region* map;

	size_t entries;
	
};
    
}

#endif

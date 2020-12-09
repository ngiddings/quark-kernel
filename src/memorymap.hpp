#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "systypes.hpp"

namespace kernel
{

class MemoryMap
{
public:

	enum Type
	{
		AVAILABLE = 1,
		UNAVAILABLE = 2,
		ACPI = 3,
		DEFECTIVE = 5
	};

	class Region
	{
	public:

		Region();

        Region(Region& copy);

		Region(physaddr_t location, size_t size, Type type);

		const Region& operator=(const Region& rhs);

        const bool operator==(const Region& rhs) const;

        const bool operator<(const Region& rhs) const;

        const bool operator>(const Region& rhs) const;

        const bool operator<=(const Region& rhs) const;

        const bool operator>=(const Region& rhs) const;

		physaddr_t getLocation() const;

		size_t getSize() const;

		Type getType() const;

        physaddr_t getEnd() const;

		bool contains(const Region& r) const;

		bool contains(physaddr_t location, size_t size) const;

        bool overlaps(const Region& r) const;

        bool bordersLeft(const Region& right) const;

        bool bordersRight(const Region& left) const;

        bool borders(const Region& r) const;

		void truncateLeft(physaddr_t left);

		void truncateRight(physaddr_t right);
			
	private:

		physaddr_t m_location;
			
		size_t m_size;
			
		size_t m_type;
			
	};
		
	MemoryMap();

	MemoryMap(MemoryMap& copy);

	const Region& operator[](size_t index) const;

	size_t size() const;

	size_t totalMemory() const;

	void insertEntry(physaddr_t location, size_t size, Type type);

private:

	void remove(unsigned int index);

	static const unsigned int maxEntries = 16;

	Region m_map[maxEntries];

	size_t m_entries;
		
};

}

#endif
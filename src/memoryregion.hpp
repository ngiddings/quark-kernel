#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H

#include "systypes.hpp"

class MemoryRegion
{
public:

	MemoryRegion();

    MemoryRegion(const MemoryRegion& copy);

	MemoryRegion(physaddr_t location, size_t size, unsigned int type);

	const MemoryRegion& operator=(const MemoryRegion& rhs);

	/**
	 * @brief Tests whether this object describes the same region of memory
	 * as rhs, irrespective of type.
	 * 
	 * @param rhs The object to compare to
	 * @return true if and only if the location and size of the two regions are equal
	 * @return false if the two regions have differing locations or differing sizes
	 */
    bool operator==(const MemoryRegion& rhs) const;

	/**
	 * @brief Tests whether this object describes a different region of memory
	 * than rhs, irrespective of type.
	 * 
	 * @param rhs The object to compare to
	 * @return true if the two regions have differing locations or differing sizes
	 * @return false if and only if the location and size of the two regions are equal
	 */
	bool operator!=(const MemoryRegion& rhs) const;

	/**
	 * @brief 
	 * 
	 * @param rhs 
	 * @return true 
	 * @return false 
	 */
    bool operator<(const MemoryRegion& rhs) const;

    bool operator>(const MemoryRegion& rhs) const;

    bool operator<=(const MemoryRegion& rhs) const;

    bool operator>=(const MemoryRegion& rhs) const;

	physaddr_t getLocation() const;

	size_t getSize() const;

	unsigned int getType() const;

    physaddr_t getEnd() const;

	bool contains(const MemoryRegion& r) const;

	bool contains(physaddr_t location, size_t size) const;

    bool overlaps(const MemoryRegion& r) const;

    bool bordersLeft(const MemoryRegion& right) const;

    bool bordersRight(const MemoryRegion& left) const;

    bool borders(const MemoryRegion& r) const;

	void truncateLeft(physaddr_t left);

	void truncateRight(physaddr_t right);
			
private:

	physaddr_t m_location;
			
	size_t m_size;
			
	unsigned int m_type;
			
};

#endif
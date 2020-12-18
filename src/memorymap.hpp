#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "systypes.hpp"
#include "memoryregion.hpp"

namespace kernelns
{

class MemoryMap
{
public:
		
	MemoryMap();

	MemoryMap(MemoryMap& copy);

	const MemoryRegion& operator[](size_t index) const;

	/**
	 * @return The number of memory regions described by this object
	 */
	size_t size() const;

	/**
	 * @brief Get the total amount of memory described by this memory map.
	 * Since no two regions may overlap, this is equivalent to the sum of the
	 * size of each memory region stored in this object.
	 * 
	 * @return The total amount of memory described by this memory map.
	 */
	size_t totalMemory() const;

	/**
	 * @brief Get the total amount of memory of a specified type described by
	 * this memory map. Since no two regions overlap, this is equivalent to the
	 * sum of the size of each memory region stored in this object whose type
	 * is equal to `type`.
	 * 
	 * @param type The type of memory region to count
	 * @return The total amount of memory of the specified type 
	 */
	size_t totalMemory(unsigned int type) const;

	/**
	 * @brief Checks if `region` overlaps any memory region contained in this
	 * memory map.
	 * 
	 * @param region 
	 * @return true if, for any region `k` in this map, `k.overlaps(region)`.
	 * @return false if there is no region in this map where `k.overlaps(region)`.
	 */
	bool overlaps(const MemoryRegion& region) const;

	/**
	 * @brief Checks if `region` is contained an any memory region in this map
	 * with the same type as `region`.
	 * 
	 * @param region 
	 * @return true if, for any region `k` in this map, `k.contains(region)
	 * && k.getType() == region.getType()`.
	 * @return false if the condition above is not true
	 */
	bool contains(const MemoryRegion& region) const;

	/**
	 * @brief Adds `region` into the memory map. Adjusts the map as necessary
	 * so that no regions overlap. Regions of higher type take precedence over
	 * regions with lower types; therefore, the lower typed regions are trimmed
	 * or deleted where they overlap with higher typed regions. Adjacent or
	 * overlapping regions with identical types are merged.
	 * 
	 * @param region The memory region to insert
	 */
	void insertEntry(const MemoryRegion& region);

	/**
	 * @brief Overload of `insertEntry()`. @see insertRegion(const MemoryRegion&).
	 * 
	 * @param location 
	 * @param size 
	 * @param type 
	 */
	void insertEntry(physaddr_t location, size_t size, unsigned int type);

private:

	/**
	 * @brief Removes the element at `index` in `m_map`. All objects located
	 * after that index are moved accordingly.
	 * 
	 * @param index Locatation of the object to delete
	 */
	void remove(unsigned int index);

	/**
	 * @brief Inserts a copy of `region` into `m_map`. The new element is
	 * inserted such that `m_map` is sorted in ascending order. Overlapping
	 * elements are not trimmed.
	 * 
	 * @param region The memory region to insert
	 */
	void simpleInsert(const MemoryRegion& region);

	/**
	 * @brief Overload of `simpleInsert(const MemoryRegion& region)`.
	 * 
	 * @param location 
	 * @param size 
	 * @param type 
	 */
	void simpleInsert(physaddr_t location, size_t size, unsigned int type);

	/**
	 * @brief If `m_map[index].overlaps(m_map[index+1])`, the two elements are
	 * modified so that they no longer overlap. The following rules apply:
	 * 
	 * 1. This memory map will describe the same set of locations before and
	 * after performing this operation.
	 * 
	 * 2. Adjacent regions of the same type are merged into a single region.
	 * 
	 * 3. Where two regions of differing types overlap, the higher type takes
	 * precedence. The region of the lower type is truncated, split, or deleted
	 * so as to not overlap the region of the higher type.
	 * 
	 * 4. This operation is local: only `m_map[index]` and `m_map[index+1]` are
	 * modified (although if a region must be split, a new region will be 
	 * inserted at the location `index+2`).
	 * 
	 * This function has no effect if `index+1 >= m_entries`, or if 
	 * `m_map[index]` does not overlap the `m_map[index+1]`.
	 * 
	 * @param index The location of the region to trim.
	 * 
	 * @return The index of the rightmost region affected.
	 */
	unsigned int trim(unsigned int index);

	static const unsigned int maxEntries = 16;

	static const unsigned int invalidIndex = 0xFFFF;

	MemoryRegion m_map[maxEntries];

	size_t m_entries;
		
};

}

#endif
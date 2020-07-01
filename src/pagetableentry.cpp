/*
 * PageTableEntry.cpp
 *
 *  Created on: May 22, 2019
 *      Author: nathan
 */

#include "pagetableentry.h"

namespace qkernel {

static_assert(sizeof(PageTableEntry) == 4, "PTE structure is the wrong size!");

PageTableEntry::PageTableEntry() {
	this->present = 0;
	this->rw = 0;
	this->usermode = 0;
	this->writeThrough = 0;
	this->cacheDisable = 0;
	this->accessed = 0;
	this->dirty = 0;
	this->pat = 0;
	this->global = 0;
	this->shared = 0;
	this->ignored = 0;
	this->physicalAddress = 0;
}

uint32_t PageTableEntry::getAccessed() const {
	return accessed;
}

uint32_t PageTableEntry::getCacheDisable() const {
	return cacheDisable;
}

void PageTableEntry::setCacheDisable(uint32_t cacheDisable)
{
	this->cacheDisable = cacheDisable;
}

uint32_t PageTableEntry::getDirty() const {
	return dirty;
}

uint32_t PageTableEntry::getGlobal() const {
	return global;
}

void PageTableEntry::setGlobal(uint32_t global)
{
	this->global = global;
}

uint32_t PageTableEntry::getPat() const {
	return pat;
}

void PageTableEntry::setPat(uint32_t pat)
{
	this->pat = pat;
}

uint32_t PageTableEntry::getPhysicalAddress() const {
	uint32_t physicalAddress = this->physicalAddress;
	return physicalAddress << 12;
}

uint32_t PageTableEntry::setPhysicalAddress(uint32_t physicalAddress)
{
	if(physicalAddress % 4096 == 0)
	{
		this->physicalAddress = physicalAddress >> 12;
		return this->physicalAddress;
	}
	else
	{
		this->physicalAddress = !physicalAddress;
		return this->physicalAddress;
	}
}

uint32_t PageTableEntry::getPresent() const {
	return present;
}

void PageTableEntry::setPresent(uint32_t present)
{
	this->present = present;
}

uint32_t PageTableEntry::getRw() const {
	return rw;
}

void PageTableEntry::setRw(uint32_t rw)
{
	this->rw = rw;
}

uint32_t PageTableEntry::getUsermode() const {
	return usermode;
}

void PageTableEntry::setUsermode(uint32_t usermode)
{
	this->usermode = usermode;
}

uint32_t PageTableEntry::getWriteThrough() const {
	return writeThrough;
}

uint32_t PageTableEntry::getShared() const {
	return shared;
}

void PageTableEntry::setShared(uint32_t shared) {
	this->shared = shared;
}

void PageTableEntry::setWriteThrough(uint32_t writeThrough)
{
	this->writeThrough = writeThrough;
}

} /* namespace qkernel */

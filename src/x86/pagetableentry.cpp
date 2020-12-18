#include "pagetableentry.hpp"

PageTableEntry::PageTableEntry()
{
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

bool PageTableEntry::getAccessed() const
{
    return accessed == 1;
}

bool PageTableEntry::getCacheDisable() const
{
    return cacheDisable == 1;
}

void PageTableEntry::setCacheDisable(bool cacheDisable)
{
    this->cacheDisable = cacheDisable ? 1 : 0;
}

bool PageTableEntry::getDirty() const
{
    return dirty == 1;
}

bool PageTableEntry::getGlobal() const
{
    return global == 1;
}

void PageTableEntry::setGlobal(bool global)
{
    this->global = global ? 1 : 0;
}

bool PageTableEntry::getPat() const
{
    return pat == 1;
}

void PageTableEntry::setPat(bool pat)
{
    this->pat = pat ? 1 : 0;
}

physaddr_t PageTableEntry::getPhysicalAddress() const
{
    physaddr_t physicalAddress = this->physicalAddress;
    return physicalAddress << 12;
}

physaddr_t PageTableEntry::setPhysicalAddress(physaddr_t physicalAddress)
{
    this->physicalAddress = physicalAddress >> 12;
    return this->physicalAddress << 12;
}

bool PageTableEntry::getPresent() const
{
    return present == 1;
}

void PageTableEntry::setPresent(bool present)
{
    this->present = present ? 1 : 0;
}

bool PageTableEntry::getRw() const
{
    return rw == 1;
}

void PageTableEntry::setRw(bool rw)
{
    this->rw = rw ? 1 : 0;
}

bool PageTableEntry::getShared() const
{
    return shared == 1;
}

void PageTableEntry::setShared(bool shared)
{
    this->shared = shared ? 1 : 0;
}

bool PageTableEntry::getUsermode() const
{
    return usermode == 1;
}

void PageTableEntry::setUsermode(bool usermode)
{
    this->usermode = usermode ? 1 : 0;
}

bool PageTableEntry::getWriteThrough() const
{
    return writeThrough == 1;
}

void PageTableEntry::setWriteThrough(bool writeThrough)
{
    this->writeThrough = writeThrough ? 1 : 0;
}

physaddr_t PageTableEntry::operator=(physaddr_t rhs)
{
    return setPhysicalAddress(rhs);
}

PageTableEntry PageTableEntry::operator=(PageTableEntry rhs)
{
    uint32_t *iThis = (uint32_t *)this;
    uint32_t *iThat = (uint32_t *)&rhs;
    *iThis = *iThat;
    return rhs;
}
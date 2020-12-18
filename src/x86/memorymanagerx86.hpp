#ifndef MEMORYMANAGERX86_H
#define MEMORYMANAGERX86_H

#include "../memorymanager.hpp"
#include "pagetableentry.hpp"

class MemoryManagerx86 : public MemoryManager
{
public:

    virtual unsigned int getPageSize() const;

    virtual physaddr_t createAddressSpace();

    virtual void loadAddressSpace(physaddr_t table);

    virtual int mapPage(void* page, physaddr_t frame, int flags);

    virtual physaddr_t unmapPage(void* page);

private:

    static const unsigned int pageSize = 4096;

    PageTableEntry *m_pageTables, *m_pageDirectory;

};

#endif
#include "../mmap.hpp"
#include "../kernelstate.hpp"

class PageTableEntry {
public:

	PageTableEntry() 
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

	bool getAccessed() const 
	{
		return accessed == 1;
	}

	bool getCacheDisable() const
	{
		return cacheDisable == 1;
	}

	void setCacheDisable(bool cacheDisable)
	{
		this->cacheDisable = cacheDisable ? 1 : 0;
	}

	bool getDirty() const
	{
		return dirty == 1;
	}

	bool getGlobal() const
	{
		return global == 1;
	}

	void setGlobal(bool global)
	{
		this->global = global ? 1 : 0;
	}

	bool getPat() const
	{
		return pat == 1;
	}

	void setPat(bool pat)
	{
		this->pat = pat ? 1 : 0;
	}

	physaddr_t getPhysicalAddress() const
	{
		physaddr_t physicalAddress = this->physicalAddress;
		return physicalAddress << 12;
	}

	physaddr_t setPhysicalAddress(physaddr_t physicalAddress)
	{
		this->physicalAddress = physicalAddress >> 12;
		return this->physicalAddress << 12;
	}

	bool getPresent() const
	{
		return present == 1;
	}

	void setPresent(bool present)
	{
		this->present = present ? 1 : 0;
	}

	bool getRw() const
	{
		return rw == 1;
	}

	void setRw(bool rw)
	{
		this->rw = rw ? 1 : 0;
	}

	bool getShared() const
	{
		return shared == 1;
	}

	void setShared(bool shared)
	{
		this->shared = shared ? 1 : 0;
	}

	bool getUsermode() const
	{
		return usermode == 1;
	}

	void setUsermode(bool usermode)
	{
		this->usermode = usermode ? 1 : 0;
	}

	bool getWriteThrough() const
	{
		return writeThrough == 1;
	}

	void setWriteThrough(bool writeThrough)
	{
		this->writeThrough = writeThrough ? 1 : 0;
	}

	physaddr_t operator=(physaddr_t rhs)
	{
		return setPhysicalAddress(rhs);
	}

	PageTableEntry operator=(PageTableEntry rhs)
	{
		uint32_t* iThis = (uint32_t*) this;
		uint32_t* iThat = (uint32_t*) &rhs;
		*iThis = *iThat;
		return rhs;
	}

private:
	uint32_t present : 1;
	uint32_t rw : 1;
	uint32_t usermode : 1;
	uint32_t writeThrough : 1;
	uint32_t cacheDisable : 1;
	uint32_t accessed : 1;
	uint32_t dirty : 1;
	uint32_t pat : 1;
	uint32_t global : 1;
	uint32_t shared : 1;
	uint32_t ignored : 2;
	uint32_t physicalAddress : 20;
};

int kernel::mmap(void* start, size_t length, int flags)
{
    if((size_t) start % 4096 != 0)
        return -1;
    PageTableEntry* pageTables = (PageTableEntry*) 0xFFC00000;
    PageTableEntry* pageDirectory = (PageTableEntry*) 0xFFFFF000;
    size_t tableIndex = (size_t) start / 4096;
    for(int i = (int) length; i > 0; i -= 4096)
	{
		size_t directoryIndex = tableIndex / 1024;
		if(!pageDirectory[directoryIndex].getPresent())
		{
			physaddr_t newPT = State::pageAllocator.allocate(4096);
			if(newPT == 0)
				return -1;
			pageDirectory[directoryIndex] = newPT;
			pageDirectory[directoryIndex].setPresent(true);
			pageDirectory[directoryIndex].setUsermode(false);
			pageDirectory[directoryIndex].setRw(true);
		}
		if(!pageTables[tableIndex].getPresent())
		{
			physaddr_t page = State::pageAllocator.allocate(4096);
			pageTables[tableIndex] = page;
			pageTables[tableIndex].setPresent(true);
			pageTables[tableIndex].setUsermode(false);
			if(flags & MMAP_RW)
				pageTables[tableIndex].setRw(true);
			
		}
		tableIndex++;
	}
	return 0;
}

int kernel::munmap(void* start, size_t length)
{
    if((size_t) start % 4096 != 0)
		return -1;
    PageTableEntry* pageTables = (PageTableEntry*) 0xFFC00000;
    PageTableEntry* pageDirectory = (PageTableEntry*) 0xFFFFF000;
	size_t tableIndex = (size_t) start / 4096;
	for(int i = (int) length; i > 0; i -= 4096)
	{
		size_t directoryIndex = tableIndex / 1024;
		if(pageDirectory[directoryIndex].getPresent())
		{
			pageTables[tableIndex] = 0;
			pageTables[tableIndex].setPresent(false);
			pageTables[tableIndex].setUsermode(false);
			pageTables[tableIndex].setRw(false);
		}
		tableIndex++;
	}
	return 0;
}

bool kernel::isMapped(void* addr)
{
    PageTableEntry* pageDirectory = (PageTableEntry*) 0xFFFFF000;
    size_t tableIndex = (size_t) addr / 4096;
    size_t directoryIndex = tableIndex / 1024;
    if(pageDirectory[directoryIndex].getPresent())
    {
        PageTableEntry* pageTables = (PageTableEntry*) 0xFFC00000;
        return pageTables[tableIndex].getPresent();
    }
    return false;
}

physaddr_t kernel::getPhysicalAddress(void* addr)
{
    PageTableEntry* pageTables = (PageTableEntry*) 0xFFC00000;
    size_t tableIndex = (size_t) addr / 4096;
    return pageTables[tableIndex].getPhysicalAddress() + ((size_t) addr & 0xFFF);
}

int kernel::createAddressSpace(void* table)
{
	if(((size_t) table & 0xFFF) != 0)
		return -1;
    PageTableEntry* pageDirectory = (PageTableEntry*) 0xFFFFF000;
    PageTableEntry* newDirectory = (PageTableEntry*) table;
    newDirectory[1022] = pageDirectory[1022];
    newDirectory[1023] = pageDirectory[1023];
	return 0;
}

int kernel::loadAddressSpace(physaddr_t table)
{
	if((table & 0xFFF) != 0)
		return -1;
	asm("mov %0, %%cr3"
		: 
		: "r" (table));
	return 0;
}
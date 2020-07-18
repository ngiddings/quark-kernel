#ifndef SRC_PAGETABLEENTRY_H_
#define SRC_PAGETABLEENTRY_H_

#include <stdint.h>
#include "../types.hpp"

namespace kernel {

class PageTableEntry {
public:
	PageTableEntry();
	bool getAccessed() const;
	bool getCacheDisable() const;
	void setCacheDisable(bool cacheDisable);
	bool getDirty() const;
	bool getGlobal() const;
	void setGlobal(bool global);
	bool getPat() const;
	void setPat(bool pat);
	physaddr_t getPhysicalAddress() const;
	physaddr_t setPhysicalAddress(physaddr_t physicalAddress);
	bool getPresent() const;
	void setPresent(bool present);
	bool getRw() const;
	void setRw(bool rw);
	bool getShared() const;
	void setShared(bool shared);
	bool getUsermode() const;
	void setUsermode(bool usermode);
	bool getWriteThrough() const;
	void setWriteThrough(bool writeThrough);
	physaddr_t operator=(physaddr_t rhs);

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

}

#endif

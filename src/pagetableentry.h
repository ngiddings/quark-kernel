/*
 * PageTableEntry.h
 *
 *  Created on: May 22, 2019
 *      Author: nathan
 */

#ifndef SRC_PAGETABLEENTRY_H_
#define SRC_PAGETABLEENTRY_H_

#include <stdint.h>

namespace qkernel {

class PageTableEntry {
public:
	PageTableEntry();
	uint32_t getAccessed() const;
	uint32_t getCacheDisable() const;
	void setCacheDisable(uint32_t cacheDisable);
	uint32_t getDirty() const;
	uint32_t getGlobal() const;
	void setGlobal(uint32_t global);
	uint32_t getPat() const;
	void setPat(uint32_t pat);
	uint32_t getPhysicalAddress() const;
	uint32_t setPhysicalAddress(uint32_t physicalAddress);
	uint32_t getPresent() const;
	void setPresent(uint32_t present);
	uint32_t getRw() const;
	void setRw(uint32_t rw);
	uint32_t getShared() const;
	void setShared(uint32_t shared);
	uint32_t getUsermode() const;
	void setUsermode(uint32_t usermode);
	uint32_t getWriteThrough() const;
	void setWriteThrough(uint32_t writeThrough);

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

} /* namespace qkernel */

#endif /* SRC_PAGETABLEENTRY_H_ */

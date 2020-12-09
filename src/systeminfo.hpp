#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <stddef.h>

#include "systypes.hpp"
#include "memorymap.hpp"

namespace kernel
{

class SystemInfo
{
public:

	SystemInfo();

	SystemInfo(MemoryMap& memoryMap, const char* commandLine);

	const MemoryMap& getMemoryMap() const;

	const char* getCommandLine() const;

private:

	MemoryMap m_memmap;

	char commandLine[128];
	
};
	
}

#endif

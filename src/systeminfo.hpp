#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <stddef.h>

#include "systypes.hpp"

namespace kernel
{

class SystemInfo
{
public:

	size_t getLowMemory();

	size_t getHighMemory();

	physaddr_t getKernelBase();

private:
    
	size_t lowMemory;

	size_t highMemory;

	physaddr_t kernelBase;

	physaddr_t kernelEnd;
	
};
	
}

#endif

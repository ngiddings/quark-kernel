#include "systeminfo.hpp"

size_t kernel::SystemInfo::getLowMemory()
{
	return lowMemory;
}

size_t kernel::SystemInfo::getHighMemory()
{
	return highMemory;
}

physaddr_t kernel::SystemInfo::getKernelBase()
{
	return kernelBase;
}

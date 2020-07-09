#include "systeminfo.hpp"

size_t qkernel::SystemInfo::getLowMemory()
{
	return lowMemory;
}

size_t qkernel::SystemInfo::getHighMemory()
{
	return highMemory;
}

physaddr_t qkernel::SystemInfo::getKernelBase()
{
	return kernelBase;
}

#include "systeminfo.hpp"
#include "util.hpp"

using namespace kernelns;

SystemInfo::SystemInfo()
{
	
}

SystemInfo::SystemInfo(MemoryMap& memoryMap, const char* commandLine)
	: m_memmap(memoryMap)
{
	strcpy(this->commandLine, commandLine);
}

const MemoryMap& SystemInfo::getMemoryMap() const
{
	return m_memmap;
}

const char* SystemInfo::getCommandLine() const
{
	return commandLine;
}

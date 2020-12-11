#include "module.hpp"
#include "util.hpp"

using namespace kernelns;

Module::Module()
{
    m_start = 0;
    m_end = 0;
    m_command = NULL;
}

Module::Module(physaddr_t start, physaddr_t end, const char* command)
{
    m_start = start;
    m_end = end;
    m_command = new char[strlen(command) + 1];
    strcpy(m_command, command);
}

Module::~Module()
{
    delete[] m_command;
}

physaddr_t Module::getStart() const
{
    return m_start;
}

physaddr_t Module::getEnd() const
{
    return m_end;
}

const char* Module::getCommand() const
{
    return m_command;
}
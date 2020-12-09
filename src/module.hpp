#ifndef ELF_H
#define ELF_H

#include <stdint.h>

#include "systypes.hpp"

namespace kernel
{

class Module
{
public:

    Module();

    Module(physaddr_t start, physaddr_t end, const char* command);

    ~Module();

    physaddr_t getStart() const;

    physaddr_t getEnd() const;

    const char* getCommand() const;

private:

    physaddr_t m_start;

    physaddr_t m_end;

    char* m_command;

};

}

#endif
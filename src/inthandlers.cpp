#include "inthandlers.hpp"

extern "C"
__attribute__ ((interrupt))
void gpFaultHandler(void* frame, unsigned int error)
{

}

extern "C"
__attribute__ ((interrupt))
void pageFaultHandler(unsigned int error)
{

}

}

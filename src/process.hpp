#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>

namespace kernel
{

class Process
{
public:

    Process();

    size_t priority;

    void* stack;

};

};

#endif
#ifndef KERNELSTATE_H
#define KERNELSTATE_H

#include "pageallocator.hpp"
#include "allocator.hpp"
#include "interrupts.hpp"
#include "scheduler.hpp"

namespace kernel
{

class State
{
public:

    static const unsigned int MAX_PROCESSES = 2048;

    static BuddyAllocator pageAllocator;

    static Allocator allocator;

    static Interrupts interrupts;

    static ProcessQueue processQueue;

};

}

#endif
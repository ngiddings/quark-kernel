#include "kernelstate.hpp"

using namespace kernelns;

BuddyAllocator State::pageAllocator;
Allocator State::allocator;
Interrupts State::interrupts;
ProcessQueue State::processQueue;
SystemInfo State::config;
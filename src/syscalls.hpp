#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>
#include "systypes.hpp"

int mmap(void* location, size_t length, int flags);

int munmap(void* location, size_t length);

unsigned int createSharedBlock(void* location, size_t length, int flags);

int aquireSharedBlock(void* location, unsigned int id);

int releaseSharedBlock(int id);

int querySharedBlock(void* info, int id);

int aquirePhysicalBlock(void* location, physaddr_t physicalAddress, size_t length);

int releasePhysicalBlock(int id);

#endif
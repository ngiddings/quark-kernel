#ifndef MEM_TYPES_H
#define MEM_TYPES_H

#include <stdint.h>
#include <stddef.h>

#if defined __i386__
typedef uint32_t physaddr_t;
#elif defined __amd64__
typedef uint64_t physaddr_t;
#else
typedef uint64_t physaddr_t;
#endif

#endif

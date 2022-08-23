#ifndef _QUARK_PHYSADDR_H
#define _QUARK_PHYSADDR_H

#include <stdint.h>

#if defined __i386__ || __arm__
typedef uint32_t physaddr_t;
#elif defined __x86_64__ || __aarch64__
typedef uint64_t physaddr_t;
#else
typedef uint64_t physaddr_t;
#endif

#endif
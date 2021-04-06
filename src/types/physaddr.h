#ifndef PHYSADDR_H
#define PHYSADDR_H

#include <stdint.h>
#include <stddef.h>

#if defined __i386__ || __arm__
typedef uint32_t physaddr_t;
#elif defined __x86_64__ || __aarch64__
typedef uint64_t physaddr_t;
#else
typedef uint64_t physaddr_t;
#endif

#endif
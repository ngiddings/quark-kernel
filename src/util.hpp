#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

extern "C" void __cxa_pure_virtual();

extern "C" void* memcpy(void* destination, const void* source, size_t num);

extern "C" void* memmove(void* destination, const void* source, size_t num);

extern "C" int memcmp(const void* ptr1, const void* ptr2, size_t num);

extern "C" void* memset(void* ptr, int value, size_t num);

extern "C" int strlen(const char* str);

extern "C" char* strcpy(char* destination, const char* source);

#endif
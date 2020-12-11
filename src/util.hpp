#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

extern "C" void* memcpy(void* destination, const void* source, size_t num);

extern "C" void* memmove(void* destination, const void* source, size_t num);

extern "C" int memcmp(const void* ptr1, const void* ptr2, size_t num);

extern "C" void* memset(void* ptr, int value, size_t num);

extern "C" int strlen(const char* str);

extern "C" char* strcpy(char* destination, const char* source);

extern "C" void* malloc(size_t size);

extern "C" void* calloc(size_t count, size_t size);

extern "C" void* realloc(void* ptr, size_t size);

extern "C" void free(void* p);

extern "C" void __cxa_pure_virtual();

void* operator new(size_t size);
 
void* operator new[](size_t size);
 
void operator delete(void* p);
 
void operator delete[](void* p);

inline void* operator new(size_t, void *p) 
{ 
    return p; 
}

inline void* operator new[](size_t, void *p) 
{ 
    return p; 
}

inline void operator delete(void *, void *) 
{ 

}

inline void operator delete[](void *, void *) 
{ 

}

#endif
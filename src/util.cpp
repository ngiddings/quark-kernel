#include <stdint.h>

#include "util.hpp"
#include "kernelstate.hpp"

void* memcpy(void* destination, const void* source, size_t num)
{
	if(num > 0)
	{
		const uint8_t* src = (const uint8_t*) source;
		uint8_t* dest = (uint8_t*) destination;
		for(size_t i = 0; i < num; i++)
		{
			dest[i] = src[i];
		}
	}
	return destination;
}

void* memmove(void* destination, const void* source, size_t num)
{
	if(num > 0)
	{
		if(((size_t) destination) < ((size_t) source) || ((size_t) destination) >= (((size_t) source)) + num)
		{
			return memcpy(destination, source, num);
		}
		else if(destination != source)
		{
			size_t i = num - 1;
			const uint8_t* src = (const uint8_t*) source;
			uint8_t* dest = (uint8_t*) destination;
			do
			{
				dest[i] = src[i];
				i--;
			} while(i != 0);
		}
	}
	return destination;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num)
{
	const uint8_t* a = (const uint8_t*) ptr1;
	const uint8_t* b = (const uint8_t*) ptr2;
	for(size_t i = 0; i < num; i++)
	{
		if(a[i] < b[i])
			return -1;
		else if(a[i] > b[i])
			return 1;
	}
	return 0;
}

void* memset(void* ptr, int value, size_t num)
{
	uint8_t* dest = (uint8_t*) ptr;
	uint8_t v = (uint8_t) value;
	for(size_t i = 0; i < num; i++)
	{
		dest[i] = v;
	}
	return ptr;
}

int strlen(const char* str)
{
	int i = 0;
	while(str[i] != '\0')
	{
		i++;
	}
	return i;
}

char* strcpy(char* destination, const char* source)
{
	int sourceLen = strlen(source);
	if((destination <= source) || (destination > (source + sourceLen)))
	{
		char* d = destination;
		const char* s = source;
		while(true)
		{
			*d = *s;
			if(*s == '\0')
			{
				break;
			}
			else
			{
				s++;
				d++;
			}
		}
	}
	else
	{
		char* d = destination + sourceLen;
		const char* s = source + sourceLen;
		do
		{
			*d = *s;
			d--;
			s--;
		} while(d > destination);
	}
	return destination;
}

void* malloc(size_t size)
{
	return kernelns::State::allocator.allocate(size);
}

void* calloc(size_t count, size_t size)
{
	return memset(malloc(count * size), 0, count * size);
}

void* realloc(void* ptr, size_t size)
{
	void* n = kernelns::State::allocator.allocate(size);
	memmove(n, ptr, size);
	free(ptr);
	return n;
}

void free(void* p)
{
	kernelns::State::allocator.free(p);
}

void __cxa_pure_virtual()
{
    // do nothing
}
 
void* operator new(size_t size)
{
    return malloc(size);
}
 
void* operator new[](size_t size)
{
    return malloc(size);
}
 
void operator delete(void* p)
{
    free(p);
}
 
void operator delete[](void* p)
{
    free(p);
}
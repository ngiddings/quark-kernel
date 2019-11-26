/*
 * cstring.h
 *
 *  Created on: Jun 13, 2019
 *      Author: nathan
 */

#ifndef SRC_CSTRING_H_
#define SRC_CSTRING_H_

#include <stddef.h>

extern "C"
{

void* memcpy(void* destination, const void* source, size_t num);

void* memmove(void* destination, const void* source, size_t num);

int memcmp(const void* ptr1, const void* ptr2, size_t num);

void* memset (void* ptr, int value, size_t num);

}

#endif /* SRC_CSTRING_H_ */

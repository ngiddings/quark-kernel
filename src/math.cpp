/*
 * math.cpp
 *
 *  Created on: May 23, 2019
 *      Author: nathan
 */

#include "math.h"

uint32_t ilog2(uint32_t n, bool roundUp)
{
	uint32_t m = n;
	uint32_t count = 0;
	bool isPowerOfTwo = true;
	while(m)
	{
		if((m & 1) == 1 && m > 1)
		{
			isPowerOfTwo = false;
		}
		count++;
		m >>= 1;
	}
	return count - (isPowerOfTwo ? 1 : (roundUp ? 0 : 1));
}



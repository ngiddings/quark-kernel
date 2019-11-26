#include <stddef.h>
#include <stdint.h>

#if __STDC_HOSTED__ == 1 || __i686__ != 1
#error "ERROR: This program must be compiled for a freestanding environment, and currently only supports the i686 target."
#endif

void main()
{
	int x = 2;
	x -= 1;
	return;
}

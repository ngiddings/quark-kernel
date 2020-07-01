#include <stddef.h>
#include <stdint.h>

#include "tty.h"

#if __STDC_HOSTED__ == 1 || __i686__ != 1
#error "ERROR: This program must be compiled for a freestanding environment, and currently only supports the i686 target."
#endif

#include "buddyallocator.h"

using namespace qkernel;

void main(void* bootInfo)
{
	char* vga = (char*) 0xC0400000;
        TTY tty(vga);
	tty << "--Quark Kernel--\n";
	tty << "Successfully enabled paging. Kernel image mapped to 0xC0000000.\n";
	tty << "Nothing left to do. Hanging.\n";
	tty << 18542;
	char c = 'b';
      	return;
}

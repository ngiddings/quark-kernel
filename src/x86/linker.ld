ENTRY(_start)

SECTIONS
{
	. = 0xFF400000;
	VIRTUAL_BASE = .;
	PHYSICAL_BASE = 0x100000;
	BASE_DIFF = VIRTUAL_BASE - PHYSICAL_BASE;
 
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}
 
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
 
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
 
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
	
	LOAD_START = ADDR(.text) - (VIRTUAL_BASE - PHYSICAL_BASE);
	LOAD_END = ADDR(.data) + SIZEOF(.data) - (VIRTUAL_BASE - PHYSICAL_BASE);
	BSS_END = ADDR(.bss) + SIZEOF(.bss) - (VIRTUAL_BASE - PHYSICAL_BASE);
	IMAGE_SIZE = ((BSS_END - LOAD_START) + (4096 - ((BSS_END - LOAD_START) % 4096))) / 4096;

	_kernel_pstart = PHYSICAL_BASE;
	_kernel_pend = PHYSICAL_BASE + (4096 * IMAGE_SIZE);
	_kernel_start = VIRTUAL_BASE;
	_kernel_tend = VIRTUAL_BASE + SIZEOF(.text) + SIZEOF(.rodata);
	_kernel_end = VIRTUAL_BASE + (4096 * IMAGE_SIZE);
	_entry_paddr = _start - _kernel_start + _kernel_pstart;
}

.section .multiboot

/*
 * Define constants for the multiboot header. See Multiboot 2 Specifications for details.
 */
.set align, 1<<0
.set meminfo, 1<<1
.set magic, 0xE85250D6
.set arch, 0
.set headerLength, _multibootHeaderEnd - _multibootHeaderStart
.set checksum, -(magic + arch + headerLength)

.set tagNotOptional, 0

.set tagInfoRequestType, 1
.set tagInfoRequestSize, _multibootInfoTagEnd - _multibootInfoTagStart
.set requestBootCommand, 1
.set requestBootLoaderName, 2
.set requestBootModules, 3
.set requestMemoryInfo, 4
.set requestBootDevice, 5
.set requestMemoryMap, 6

.set tagAddressType, 2
.set tagAddressSize, 24
.set tagAddressHeaderLocation, LOAD_START
.set tagAddressLoadStart, LOAD_START
.set tagAddressLoadEnd, LOAD_END
.set tagAddressBSSEnd, BSS_END

.set tagEntryType, 3
.set tagEntrySize, 12
.set tagEntryAddress, _start - (0xFF800000 - 0x100000)

.set tagModuleAlignType, 6
.set tagModuleAlignSize, 8

/*
 * Each multiboot tag must be 8-byte aligned, or GRUB will not be able to read the header.
 */
.align 8
_multibootHeaderStart:

.long magic
.long arch
.long headerLength
.long checksum

.align 8

_multibootInfoTagStart:
.short tagInfoRequestType
.short tagNotOptional
.long tagInfoRequestSize
.long requestBootCommand
.long requestBootLoaderName
.long requestBootModules
.long requestMemoryInfo
.long requestBootDevice
.long requestMemoryMap
_multibootInfoTagEnd:

.align 8

.short tagAddressType
.short tagNotOptional
.long tagAddressSize
.long tagAddressHeaderLocation
.long tagAddressLoadStart
.long tagAddressLoadEnd
.long tagAddressBSSEnd

.align 8

.short tagEntryType
.short tagNotOptional
.long tagEntrySize
.long tagEntryAddress

.align 8

.short tagModuleAlignType
.short tagNotOptional
.long tagModuleAlignSize

.align 8

/*
 * Terminate list of multiboot header tags.
 * Ending tag has type = 0, flags = 0, size = 8
 */
.long 0
.long 8

_multibootHeaderEnd:
	
.section .bss

.align 16
stack_bottom:
.skip 8192
.global stack_top
stack_top:

.align 4096
.global default_page_dir
default_page_dir:
.skip 4096
default_page_table:
.skip 4096
identity_map:
.skip 4096

.section .text
.global _start
.type _start, @function
_start:
	
	cli

	# This platform reqires a Multiboot2 bootloader.
	cmp $0x36d76289, %eax
	jne .err

	# Initialize stack in physical address space
	mov $stack_top, %esp
	sub $BASE_DIFF, %esp

	# Push physical address of identity map
	mov $identity_map, %eax
	sub $BASE_DIFF, %eax
	push %eax

	# Push physical address of page table
	mov $default_page_table, %eax
	sub $BASE_DIFF, %eax
	push %eax

	# Push physical address of page directory
	mov $default_page_dir, %eax
	sub $BASE_DIFF, %eax
	push %eax

	# Push ending physical address to map
	mov (%ebx), %eax
	add %ebx, %eax
	push %eax

	# Push starting physical address to map
	mov $PHYSICAL_BASE, %eax
	push %eax

	# Load physical address of startPaging()
	mov $start_paging, %eax
	sub $BASE_DIFF, %eax

	# Initialize paging
	call *%eax
	
	# Jump into mapped kernel binary
	lea 1f, %eax
	jmp *%eax
1:

	# Flush TLB
	mov %cr3, %eax
	mov %eax, %cr3

	# Initialize stack in virtual memory
	mov $stack_top, %esp

	# Change EBX to point to the virtual address of the multiboot info
	# If the new pointer is out-of-bounds, error
	add $0xFF700000, %ebx
	cmp $0xFF800000, %ebx
	jl .err
	cmp $0xFFC00000, %ebx
	jge .err

	# Call initialize(void* multibootInfo)
	push %ebx
	call initialize

.err:
	cli
2:	hlt
	jmp 2b

.size _start, . - _start

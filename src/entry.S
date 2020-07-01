.section .multiboot
.include "src/multiboot2header.S"

.section .bss

.align 16
stackBottom:
.skip 16384
stackTop:

.align 4096
_tempPgDir:
.skip 4096
_tempIdentityMap:
.skip 4096
_tempPgTable:
.skip 8192

.section .text
.global _start
.type _start, @function
_start:
	cmp $0x36d76289, %eax
	jne _err

	movb $64, 0xB8000

	mov $0, %ecx
1:
	# Generate a page table entry pointing to a page in the kernel binary
	mov %ecx, %eax
	mov $4096, %edx
	mul %edx
	add $PHYSICAL_BASE, %eax
	or $3, %eax

	# Load the address of the temporary page table and translate it to a physical address
	mov $_tempPgTable, %edi
	sub $BASE_DIFF, %edi

	# Save the PTE into an entry in the temporary page table
	mov %eax, (%edi, %ecx, 4)

	# Load the address of the identity map and translate it to a physical address
	mov $_tempIdentityMap, %edi
	sub $BASE_DIFF, %edi

	# Save the PTE into an entry in the identity map
	mov %eax, 1024(%edi, %ecx, 4)

	# Increment count and loop
	inc %ecx
	cmp $IMAGE_SIZE, %ecx
	jne 1b

	# Load the physical address of the identity map, and generate a PDE
	mov $_tempIdentityMap, %eax
	sub $BASE_DIFF, %eax
	or $3, %eax

	# Load the physical address of the page directory
	mov $_tempPgDir, %edi
	sub $BASE_DIFF, %edi
	
	# Save the PDE to the first element in the page directory
	mov %eax, (%edi)

	# Load the physical address of the temporary page table, and generate a PDE
	mov $_tempPgTable, %eax
	sub $BASE_DIFF, %eax
	or $3, %eax

	# Save the PDE to the entry corresponding to 0xC0000000
	mov %eax, 3072(%edi)

	# Add another PDE for extra mappings the kernel needs
	add $4096, %eax
	mov %eax, 3076(%edi)
	
	# Set the last entry in the page directory to point to the page directory itself
	or $3, %edi
	mov %edi, 4092(%edi)

	# Map VGA memory into the address space
	mov $_tempPgTable, %edi
	sub $BASE_DIFF, %edi
	mov $0xB8003, %eax
	mov %eax, 4096(%edi)

	# Load the physical address of the page directory into CR3
	mov $_tempPgDir, %edi
	sub $BASE_DIFF, %edi
	mov %edi, %cr3
	
	# Enable paging
	mov %cr0, %eax
	or $0x80010000, %eax
	mov %eax, %cr0

	# Jump into mapped kernel binary
	lea 2f, %eax
	jmp *%eax
2:
	# Delete PDE corresponding to identity map. We shouldn't need it anymore.
	movl $0, (_tempIdentityMap)

	# Reload page tables
	mov %cr3, %eax
	mov %eax, %cr3

	# Initialize stack
	mov $stackTop, %esp

	push %ebx
	
	# Call main function
	call main

_err:
	cli
3:	hlt
	jmp 3b

.size _start, . - _start

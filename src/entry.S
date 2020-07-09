.section .multiboot
.include "src/multiboot2header.S"

.section .rodata
	
idt_info:
.short idt_end - idt - 1
.long idt
	
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

_bootCmdLine:
.skip 64

.align 64
.global system_info
system_info:
.skip 16

.align 64
.global memory_map
memory_map:
.skip 16 * 16

.global idt
idt:
.skip 8 * 256
idt_end:
	
.section .text
.global _start
.type _start, @function
_start:
	cmp $0x36d76289, %eax
	jne _err

	movb $64, 0xB8000

	mov $system_info, %edi
	sub $BASE_DIFF, %edi
	add $8, %ebx
	
switch:	mov (%ebx), %eax
	cmp $0, %eax
	je s_end
	cmp $1, %eax
	je tag_1
	cmp $4, %eax
	je tag_4
	cmp $6, %eax
	je tag_6
	cmp $21, %eax
	je tag_21
	jmp def

# Boot command line
tag_1:	mov 4(%ebx), %ecx
	sub $8, %ecx
	mov %ebx, %esi
	add $8, %esi
	mov $_bootCmdLine, %edi
	sub $BASE_DIFF, %edi
	rep movsb
	mov $system_info, %edi
	sub $BASE_DIFF, %edi
	jmp def
	
# Basic memory info
tag_4:	mov 8(%ebx), %eax
	mov %eax, (%edi)
	mov 12(%ebx), %eax
	mov %eax, 4(%edi)
	jmp def

# Memory map
tag_6:	mov $memory_map, %esi
	sub $BASE_DIFF, %esi	# set esi to point to the table in the kernel image
	mov 4(%ebx), %ecx
	sub $16, %ecx		# set ecx to store the size of the table provided by the bootloader
	mov 8(%ebx), %edx	# set edx to store the size of each table entry
	add $16, %ebx		# move ebx up to the first entry
1:	mov (%ebx), %eax
	mov %eax, (%esi)	# save the address of that region in memory
	mov 8(%ebx), %eax	
	mov %eax, 4(%esi)	# save the size of that region in memory
	mov 16(%ebx), %eax
	mov %eax, 8(%esi)	# save the type of memory in that region
	add $12, %esi		# move esi to the next entry in the kernel's array
	add %edx, %ebx		# move ebx to the next entry in the bootloader's array
	sub %edx, %ecx		# subtract the size of an entry from ecx.
	jnz 1b			# loop if there are entries left
	mov $0, %eax
	mov %eax, (%esi)
	mov %eax, 4(%esi)
	mov %eax, 8(%esi)
	jmp switch
	
# Program image location
tag_21:	mov 8(%ebx), %eax
	mov %eax, 8(%edi)
	jmp def
	
def:	mov 4(%ebx), %eax
	add $7, %eax
	and $0xFFFFFFF8, %eax
	add %eax, %ebx
	jmp switch
s_end:	

	movb $64, 0xB8002
	
	mov $0, %ecx
1:
	# Generate a page table entry pointing to a page in the kernel binary
	mov %ecx, %eax
	mov $4096, %edx
	mul %edx
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
	mov %eax, (%edi, %ecx, 4)

	# Increment count and loop
	inc %ecx
	mov $IMAGE_SIZE, %edx
	add $256, %edx
	cmp %edx, %ecx
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
	
	# Set the last entry in the page directory to point to the page directory itself
	mov %edi, %eax
	or $3, %eax
	mov %eax, 4092(%edi)

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

	lidt idt_info

	mov $_bootCmdLine, %eax
	push %eax
	
	# Call main function
	call main

_err:
	cli
3:	hlt
	jmp 3b

.size _start, . - _start

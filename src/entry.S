.section .multiboot
.include "multiboot2header.S"

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
.skip 4096

.section .text
.global _start
.type _start, @function
_start:
	cmp $0x36d76289, %eax
	jne _err

	movb $64, 0xB8000

	mov $0, %ecx
1:	mov %ecx, %eax
	mov $4096, %edx
	mul %edx
	add $PHYSICAL_BASE, %eax
	or $3, %eax
	mov $_tempPgTable, %edi
	mov %eax, (%edi, %ecx, 4)
	mov $_tempIdentityMap, %edi
	mov %eax, 1024(%edi, %ecx, 4)
	inc %ecx
	cmp $IMAGE_SIZE, %ecx
	jne 1b

	mov $_tempIdentityMap, %eax
	or $3, %eax
	mov %eax, (_tempPgDir)

	mov $_tempPgTable, %eax
	or $3, %eax
	mov %eax, (_tempPgDir + 3072)

	mov $_tempPgDir, %eax
	mov %eax, %cr3

	mov %cr0, %eax
	or $0x80010000, %eax
	mov %eax, %cr0
	lea 2f, %eax
	jmp *%eax
2:	movl $0, (_tempIdentityMap)
	mov %cr3, %eax
	mov %eax, %cr3

	mov $stackTop, %esp

	call main

_err:
	cli
3:	hlt
	jmp 3b

.size _start, . - _start

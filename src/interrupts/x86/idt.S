.section .bss

.align 8
.global idt
idt:
.skip 8 * 256
idt_end:

.section .rodata

.idt_info:
.short idt_end - idt - 1
.long idt

.section .text

.global _lidt
.type _lidt, @function
_lidt:

    ret
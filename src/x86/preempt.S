.section .text

.global isr_syscall
.type isr_syscall, @function
isr_syscall:
    cli
    push %edx
    push %ecx
    push %ebx
    push %eax
    push $kernel_state
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    call do_syscall
    add $0x14, %esp
    mov $0x23, %cx
    mov %cx, %ds
    mov %cx, %es
    mov %cx, %fs
    mov %cx, %gs
    iret

.global isr_preempt
.type isr_preempt, @function
isr_preempt:
    cli
    push %esp
    push $0x800
    call save_context
    add $8, %esp
    push $0x800
    push $kernel_state
    call next_process
    add $8, %esp
    push %eax
    call load_context
    
.section .text

.global isr_syscall
.type isr_syscall, @function
isr_syscall:
    cli
    mov %esp, %ebp
    mov 8(%ebp), %esi
    push %esi
    mov 12(%ebp), %esi
    push %esi
    mov (%ebp), %esi
    push %esi
    push %edx
    push %ecx
    push %ebx
    push %eax
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    call do_syscall
    mov %ebp, %esp
    mov $0x23, %cx
    mov %cx, %ds
    mov %cx, %es
    mov %cx, %fs
    mov %cx, %gs
    iret

.global isr_preempt
.type isr_preempt, @function
isr_preempt:

    // Disable interrupts for now
    cli

    // Save process's base pointer
    push %ebp

    // Save base pointer; it points to the interrupt stack frame
    mov %esp, %ebp
    add $4, %ebp

    // Save EAX on stack so it can be used as a buffer
    push %eax

    // Load EFLAGS, then push it onto stack
    mov 8(%ebp), %eax
    push %eax

    // Load EIP, then push it onto stack
    mov (%ebp), %eax
    push %eax

    // Load CS, then push it onto stack
    mov 4(%ebp), %eax
    push %eax

    // Load ESP, then push it onto stack
    mov 12(%ebp), %eax
    push %eax

    // Load SS, then push it onto stack
    mov 16(%ebp), %eax
    push %eax

    // Load EBP, then push it onto stack
    mov -4(%ebp), %eax
    push %eax

    // Push GP registers onto stack
    push %esi
    push %edi
    push %edx
    push %ecx
    push %ebx

    // Restore saved value of EAX, then push it onto stack
    mov -8(%ebp), %eax
    push %eax

    // Push pointer to the process context saved on the stack
    push %esp

    call save_context
    mov %ebp, %esp

    call kernel_advance_scheduler

    push %eax
    call load_context
    
.global _do_syscall
.type _do_syscall, @function
_do_syscall:
    push %ebp
    mov %esp, %ebp
    push %esi
    push %edi
    push %ebx
    mov 0x08(%ebp), %eax
    mov 0x0C(%ebp), %ebx
    mov 0x10(%ebp), %ecx
    mov 0x14(%ebp), %edx
    int $0x80
    pop %ebx
    pop %edi
    pop %esi
    pop %ebp
    ret
.size _do_syscall, . - _do_syscall

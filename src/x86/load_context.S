.section .text

/*
 * load_context(struct process_state_t *context)
 */
.global load_context
.type load_context, @function
load_context:
    mov 4(%esp), %eax
    push 0x1C(%eax)
    push 0x20(%eax)
    push 0x2C(%eax)
    push 0x24(%eax)
    push 0x28(%eax)
    push 0x00(%eax)
    mov 0x04(%eax), %ebx
    mov 0x08(%eax), %ecx
    mov 0x0C(%eax), %edx
    mov 0x10(%eax), %edi
    mov 0x14(%eax), %esi
    mov 0x18(%eax), %ebp
    mov 0x1C(%eax), %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    pop %eax
    iret
.section .text

/*
 * save_context(struct process_state_t *context, struct interrupt_frame_t *frame)
 */
 .global save_context
 .type save_context, @function
save_context:
    push %edi
    push %esi
    push %eax
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov 16(%esp), %eax
    mov %ebx, 0x04(%eax)
    mov %ecx, 0x08(%eax)
    mov %edx, 0x0C(%eax)
    mov %edi, 0x10(%eax)
    mov %esi, 0x14(%eax)
    mov %ebp, 0x18(%eax)
    mov %eax, %edi
    pop %eax
    mov %eax, 0x00(%edi)
    mov 16(%esp), %esi
    mov 0x10(%esi), %eax
    mov %eax, 0x1C(%edi)
    mov 0x0C(%esi), %eax
    mov %eax, 0x20(%edi)
    mov 0x08(%esi), %eax
    mov %eax, 0x2C(%edi)
    mov 0x04(%esi), %eax
    mov %eax, 0x24(%edi)
    mov 0x00(%esi), %eax
    mov %eax, 0x28(%edi)
    pop %esi
    pop %edi
    ret

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
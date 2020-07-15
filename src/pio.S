.global outb
outb:
    mov 4(%esp), %dx
    mov 8(%esp), %al
    out %al, %dx
    ret

.global inb
inb:
    mov 4(%esp), %dx
    xor %eax, %eax
    in %dx, %al
    ret
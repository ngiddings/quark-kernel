.section .text

.global isr_preempt
.type isr_preempt, @function
isr_preempt:
    cli
    push %esp
    push $0x800
    call save_context
    sub $8, %esp
    push $0x800
    push $kernel_state
    call next_process
    sub $8, %esp
    push %eax
    call load_context
    
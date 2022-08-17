#pragma once

#include <stdint.h>

enum interrupt_code_t
{
    EXCEPTION_DIV_BY_0 = 0,
    EXCEPTION_DEBUG = 1,
    EXCEPTION_NMI = 2,
    EXCEPTION_BREAKPOINT = 3,
    EXCEPTION_OVERFLOW = 4,
    EXCEPTION_OUT_OF_BOUNDS = 5,
    EXCEPTION_INVALID_OPCODE = 6,
    EXCEPTION_DEVICE_NOT_AVAILABLE = 7,
    EXCEPTION_DOUBLE_FAULT = 8,
    EXCEPTION_INVALID_TSS = 10,
    EXCEPTION_SEGMENT_NOT_PRESENT = 11,
    EXCEPTION_STACK_SEGMENT_FAULT = 12,
    EXCEPTION_GPF = 13,
    EXCEPTION_PAGE_FAULT = 14,
    EXCEPTION_x87_FLOATING_POINT = 16,
    EXCEPTION_ALIGNMENT_CHECK = 17,
    EXCEPTION_MACHINE_CHECK = 18,
    EXCEPTION_SIMD_FLOATING_POINT = 19,
    EXCEPTION_VIRTUALIZATION = 20,
    EXCEPTION_SECURITY = 30,
    ISR_APIC_TIMER = 64,
    ISR_PREEMPT = 65,
    ISR_AP_START = 127,
    ISR_SYSCALL = 128
};

enum isr_type_t
{
    INTERRPUT_TASK32 = 5,
    INTERRPUT_TRAP32 = 15,
    INTERRPUT_INT32 = 14,
    INTERRPUT_TRAP16 = 7,
    INTERRPUT_INT16 = 6
};

void initialize_gdt();

void initialize_idt();
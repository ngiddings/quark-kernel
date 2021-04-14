#pragma once

#include <stdint.h>

enum exception_code_t
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
    EXCEPTION_SECURITY = 30
};

enum isr_type_t
{
    INTERRPUT_TASK32 = 5,
    INTERRPUT_TRAP32 = 15,
    INTERRPUT_INT32 = 14,
    INTERRPUT_TRAP16 = 7,
    INTERRPUT_INT16 = 6
};

struct interrupt_descriptor_t
{
    uint16_t offset_1;
    uint16_t selector;
    uint16_t zero : 8;
    uint16_t type : 4;
    uint16_t storage : 1;
    uint16_t dpl : 2;
    uint16_t present : 1;
    uint16_t offset_2;
};

void lidt(struct interrupt_descriptor_t *idt);

void create_interrupt_descriptor(struct interrupt_descriptor_t *descriptor, void *isr, enum isr_type_t type, uint32_t privilage);

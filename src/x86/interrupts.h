#pragma once

#include <stdint.h>

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

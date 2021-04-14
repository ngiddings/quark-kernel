#include "interrupts.h"

struct idt_info_t
{
    uint16_t size;
    void *location;
};

void lidt(struct interrupt_descriptor_t *idt)
{
    struct idt_info_t idt_info;
    idt_info.size = sizeof(idt) - 1;
    idt_info.location = (void *)&idt;
    asm("lidt (%0)"
        :
        : "r"(&idt_info));
}

void create_interrupt_descriptor(struct interrupt_descriptor_t *descriptor, void *isr, enum isr_type_t type, uint32_t privilage)
{
    descriptor->offset_1 = (uint32_t) isr & 0xFFFF;
    descriptor->offset_2 = (uint32_t) isr >> 16;
    descriptor->selector = 8;
    descriptor->zero = 0;
    descriptor->type = type;
    descriptor->storage = 0;
    descriptor->dpl = privilage;
    descriptor->present = 1;
}

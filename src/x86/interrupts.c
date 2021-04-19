#include "x86/interrupts.h"
#include "x86/isr.h"
#include "string.h"
#include "system.h"
#include <stddef.h>

#define idt_size 256
#define gdt_size 6

extern int default_page_dir;

enum segment_type_t
{
    SEGMENT_KERNEL_CODE,
    SEGMENT_KERNEL_DATA,
    SEGMENT_USER_CODE,
    SEGMENT_USER_DATA,
    SEGMENT_TSS
};

struct gdt_entry_t
{
	unsigned int limit_low : 16;
	unsigned int base_low : 24;
	unsigned int accessed : 1;
	unsigned int read_write : 1;
	unsigned int conforming_expand_down : 1;
	unsigned int code : 1;
	unsigned int code_data_segment : 1;
	unsigned int dpl : 2;
	unsigned int present : 1;
	unsigned int limit_high : 4;
	unsigned int available : 1;
	unsigned int long_mode : 1;
	unsigned int big : 1;
	unsigned int granularity : 1;
	unsigned int base_high : 8;
} __attribute__ ((packed));

struct tss_entry_t
{
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
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
} __attribute__ ((packed));

struct descriptor_table_info_t
{
    uint16_t size;
    void *location;
} __attribute__ ((packed));

void load_gdt(struct gdt_entry_t *gdt)
{
    struct descriptor_table_info_t gdt_info;
    gdt_info.size = sizeof(struct gdt_entry_t) * 7 - 1;
    gdt_info.location = (void *)gdt;
    asm("lgdt (%0);"
        "jmp $8, $.ldcs;"
        ".ldcs:;"
        "mov $16, %%ax;"
        "mov %%ax, %%ds;"
        "mov %%ax, %%es;"
        "mov %%ax, %%gs;"
        "mov %%ax, %%fs;"
        "mov %%ax, %%ss;"
        :
        : "r"(&gdt_info));
}

void load_idt(struct interrupt_descriptor_t *idt)
{
    struct descriptor_table_info_t idt_info;
    idt_info.size = sizeof(struct interrupt_descriptor_t) * 256 - 1;
    idt_info.location = (void *)idt;
    asm("lidt (%0)"
        :
        : "r"(&idt_info));
}

void load_tr(uint16_t gdt_offset)
{
    gdt_offset |= 3;
    asm("mov %0, %%ax; "
        "ltr %%ax; "
        :
        : "r"(gdt_offset));
}

void create_interrupt_descriptor(struct interrupt_descriptor_t *descriptor, void *isr, enum isr_type_t type, uint32_t privilage, uint32_t selector)
{
    if(type != INTERRPUT_TASK32)
    {
        descriptor->offset_1 = (uint32_t) isr & 0xFFFF;
        descriptor->offset_2 = (uint32_t) isr >> 16;
    }
    descriptor->selector = selector;
    descriptor->zero = 0;
    descriptor->type = type;
    descriptor->storage = 0;
    descriptor->dpl = privilage;
    descriptor->present = 1;
}

void create_segment_descriptor(struct gdt_entry_t *descriptor, size_t base, size_t limit, enum segment_type_t type)
{
    descriptor->limit_low = limit;
    descriptor->limit_high = (limit & (0xf << 16)) >> 16;
    descriptor->base_low = base;
    descriptor->base_high = (base & (0xff << 24)) >> 24;
    descriptor->present = 1;
    switch(type)
    {
    case SEGMENT_KERNEL_CODE:
        descriptor->accessed = 0;
        descriptor->read_write = 1;
        descriptor->conforming_expand_down = 0;
        descriptor->code = 1;
        descriptor->code_data_segment = 1;
        descriptor->dpl = 0;
        descriptor->available = 0;
        descriptor->long_mode = 0;
        descriptor->big = 1;
        descriptor->granularity = 1;
        break;
    case SEGMENT_KERNEL_DATA:
        descriptor->accessed = 0;
        descriptor->read_write = 1;
        descriptor->conforming_expand_down = 0;
        descriptor->code = 0;
        descriptor->code_data_segment = 1;
        descriptor->dpl = 0;
        descriptor->available = 0;
        descriptor->long_mode = 0;
        descriptor->big = 1;
        descriptor->granularity = 1;
        break;
    case SEGMENT_USER_CODE:
        descriptor->accessed = 0;
        descriptor->read_write = 1;
        descriptor->conforming_expand_down = 0;
        descriptor->code = 1;
        descriptor->code_data_segment = 1;
        descriptor->dpl = 3;
        descriptor->available = 0;
        descriptor->long_mode = 0;
        descriptor->big = 1;
        descriptor->granularity = 1;
        break;
    case SEGMENT_USER_DATA:
        descriptor->accessed = 0;
        descriptor->read_write = 1;
        descriptor->conforming_expand_down = 0;
        descriptor->code = 0;
        descriptor->code_data_segment = 1;
        descriptor->dpl = 3;
        descriptor->available = 0;
        descriptor->long_mode = 0;
        descriptor->big = 1;
        descriptor->granularity = 1;
        break;
    case SEGMENT_TSS:
        descriptor->accessed = 1;
        descriptor->read_write = 0;
        descriptor->conforming_expand_down = 0;
        descriptor->code = 1;
        descriptor->code_data_segment = 0;
        descriptor->dpl = 3;
        descriptor->available = 0;
        descriptor->long_mode = 0;
        descriptor->big = 0;
        descriptor->granularity = 0;
    }
}

void initialize_gdt()
{
    static struct gdt_entry_t gdt[gdt_size];
    static struct tss_entry_t tss;
    static struct tss_entry_t double_fault_tss;
    memset(gdt, 0, sizeof(struct gdt_entry_t) * gdt_size);
    create_segment_descriptor(&gdt[1], 0, 0xFFFFF, SEGMENT_KERNEL_CODE);
    create_segment_descriptor(&gdt[2], 0, 0xFFFFF, SEGMENT_KERNEL_DATA);
    create_segment_descriptor(&gdt[3], 0, 0xFFFFF, SEGMENT_USER_CODE);
    create_segment_descriptor(&gdt[4], 0, 0xFFFFF, SEGMENT_USER_DATA);
    create_segment_descriptor(&gdt[5], (size_t)&tss, sizeof(struct tss_entry_t) - 1, SEGMENT_TSS);
    create_segment_descriptor(&gdt[6], (size_t)&double_fault_tss, sizeof(struct tss_entry_t) - 1, SEGMENT_TSS);
    memset(&tss, 0, sizeof(tss));
    memset(&double_fault_tss, 0, sizeof(tss));
    tss.esp0 = &stack_top;
    tss.ss0 = 0x10;
    double_fault_tss.esp0 = &stack_top;
    double_fault_tss.ss0 = 0x10;
    double_fault_tss.esp = &stack_top;
    double_fault_tss.cs = 0x08;
    double_fault_tss.ds = 0x10;
    double_fault_tss.ss = 0x10;
    double_fault_tss.cr3 = &default_page_dir;
    double_fault_tss.eip = (void*)isr_double_fault;
    load_gdt(gdt);
    load_tr(5 * sizeof(struct gdt_entry_t));
}

void initialize_idt()
{
    static struct interrupt_descriptor_t idt[idt_size];
    memset(idt, 0, sizeof(struct interrupt_descriptor_t) * idt_size);
    for(int i = 0; i < idt_size; i++)
    {
        create_interrupt_descriptor(&idt[i], (void*)isr_generic, INTERRPUT_INT32, 0, 8);
    }
    create_interrupt_descriptor(&idt[EXCEPTION_DIV_BY_0], (void*)isr_division_by_zero, INTERRPUT_INT32, 0, 8);
    create_interrupt_descriptor(&idt[EXCEPTION_GPF], (void*)isr_gp_fault, INTERRPUT_TRAP32, 0, 8);
    create_interrupt_descriptor(&idt[EXCEPTION_PAGE_FAULT], (void*)isr_page_fault, INTERRPUT_TRAP32, 0, 8);
    create_interrupt_descriptor(&idt[EXCEPTION_DOUBLE_FAULT], (void*)isr_double_fault, INTERRPUT_TASK32, 0, 8 * 6);
    create_interrupt_descriptor(&idt[ISR_PREEMPT], (void*)isr_preempt, INTERRPUT_INT32, 3, 8);
    create_interrupt_descriptor(&idt[ISR_APIC_TIMER], (void*)isr_timer, INTERRPUT_INT32, 0, 8);
    create_interrupt_descriptor(&idt[ISR_SYSCALL], (void*)isr_syscall, INTERRPUT_INT32, 0, 8);
    load_idt(idt);
}

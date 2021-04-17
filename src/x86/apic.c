#include "mmgr.h"
#include "apic.h"
#include "msr.h"
#include "stdio.h"
#include <stddef.h>

extern int _kernel_end;

struct apic_registers_t volatile *apic_registers;

void apic_enable(struct page_stact_t *page_stack)
{
    // Remap and mask 8259 PIC
    asm volatile(
        "mov $0x11, %%al;"
        "outb %%al, $0x20;"
        "outb %%al, $0xA0;"
        "mov $0x20, %%al;"
        "outb %%al, $0x21;"
        "mov $0x28, %%al;"
        "outb %%al, $0xA1;"
        "mov $4, %%al;"
        "outb %%al, $0x21;"
        "mov $2, %%al;"
        "outb %%al, $0xA1;"
        "mov $0x01, %%al;"
        "outb %%al, $0x21;"
        "outb %%al, $0xA1;"
        "mov $0xFF, %%al;"
        "outb %%al, $0xA1;"
        "outb %%al, $0x21;"
        ::: "al"
    );
    struct msr_apic_base_t msr;
    read_msr(MSR_APIC_BASE, (uint64_t*)&msr);
    map_page(page_stack, &_kernel_end, msr.apic_base << 12, PAGE_RW);
    printf("MSR_APIC_BASE: %016x\n", *((uint32_t*)&msr));
    apic_registers = (struct apic_registers_t*)&_kernel_end;
    apic_registers->spurious_iv.value = apic_registers->spurious_iv.value | 0x100;
}

void apic_eoi()
{
    apic_registers->eoi.value = 0;
}

void apic_send_ipi(
    uint32_t vector,
    enum apic_delivery_mode_t delivery_mode,
    enum apic_destination_mode_t destination_mode,
    enum apic_level_t level,
    enum apic_trigger_mode_t trigger_mode,
    enum apic_destination_shorthand_t shorthand,
    uint32_t destination)
{
    struct apic_icr_t value = {
        .vector = vector,
        .delivery_mode = delivery_mode,
        .destination_mode = destination_mode,
        .level = level,
        .trigger_mode = trigger_mode,
        .destination_shorthand = shorthand,
        .destination = destination
    };
    uint32_t *value_addr = (uint32_t*) &value;
    uint32_t *icr_addr = (uint32_t*)&apic_registers->interrput_command;
    icr_addr[4] = value_addr[4];
    icr_addr[0] = value_addr[0];
}

#pragma once

#include <stdint.h>

enum msr_id_t
{
    MSR_APIC_BASE = 0x1B
};

struct msr_apic_base_t
{
    uint64_t reserved_1 : 8;
    uint64_t bsp : 1;
    uint64_t reserved_2 : 1;
    uint64_t x2apic_enable : 1;
    uint64_t apic_global_enable : 1;
    uint64_t apic_base : 52;
};

void read_msr(enum msr_id_t msr_addr, uint64_t *value);

void write_msr(enum msr_id_t msr_addr, uint64_t *value);

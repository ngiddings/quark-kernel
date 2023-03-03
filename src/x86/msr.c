#include "x86/msr.h"

void read_msr(enum msr_id_t msr_addr, uint64_t *value)
{
    uint64_t v;
    asm volatile("rdmsr"
        : "=A" (v)
        : "c" (msr_addr));
    *value = v;
}

void write_msr(enum msr_id_t msr_addr, uint64_t *value)
{
    uint64_t v = *value;
    asm volatile("wrmsr"
        :: "c"(msr_addr), "A"(v));
}

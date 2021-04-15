#include "msr.h"

void read_msr(enum msr_id_t msr_addr, uint64_t *value)
{
    uint64_t v;
    asm volatile("rdmsr"
        : "=edx:eax" (v)
        : "ecx" (msr_addr));
    *value = v;
}

void write_msr(enum msr_id_t msr_addr, uint64_t *value)
{
    uint64_t v = *value;
    asm volatile("wrmsr"
        :: "ecx"(msr_addr), "A"(v));
}

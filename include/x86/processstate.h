#pragma once

#include <stdint.h>

#define PCB_LOCATION 0x800

struct process_state_t
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ss;
    uint32_t esp;
    uint32_t cs;
    uint32_t eip;
    uint32_t flags;
};

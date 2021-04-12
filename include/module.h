#pragma once

#include "types/physaddr.h"

struct module_t
{
    physaddr_t start;
    physaddr_t end;
    char string[64 - 2 * sizeof(physaddr_t)];
};
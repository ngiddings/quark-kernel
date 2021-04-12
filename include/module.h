#pragma once

#include "types/physaddr.h"

struct module_t
{
    physaddr_t start;
    physaddr_t end;
    char str[64 - 2 * sizeof(physaddr_t)];
};
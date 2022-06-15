#pragma once

#include "kernel.h"

/**
 * @brief 
 * 
 * @param boot_info 
 * @param table 
 * @return void* 
 */
void *read_multiboot_table(struct boot_info_t *boot_info, void *table);

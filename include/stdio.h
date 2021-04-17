#pragma once

#include <stdarg.h>

int initialize_screen();

int putchar(int c);

int puts(const char *str);

int printf(const char *format, ...);

int sprintf(char *str, const char *format, ...);
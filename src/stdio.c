#include "stdio.h"
#include "platform/putc.h"
#include <stddef.h>
#include <stdbool.h>

enum format_flags_t
{
    FORMAT_PADDING = '0',
    FORMAT_WIDTH = '*',

    FORMAT_SIGNED_DECIMAL = 'i',
    FORMAT_UNSIGNED_DECIMAL = 'u',
    FORMAT_UNSIGNED_OCTAL = 'o',
    FORMAT_UNSIGNED_HEX = 'x',
    FORMAT_STRING = 's',
    FORMAT_COUNT = 'n',
    FORMAT_PERCENT = '%'

};

char *itoa(unsigned int n, unsigned int base, unsigned int width)
{
    if (base < 2 || base > 16)
    {
        return NULL;
    }
    static const char *digits = "0123456789abcdef";
    static char buffer[65];
    char *s = &buffer[64];
    *s = 0;
    unsigned int count = 0;
    do
    {
        *--s = digits[n % base];
        n /= base;
        count++;
    } while (count < width || n != 0);
    return s;
}

int printf(const char *format, ...)
{
    va_list valist;
    va_start(valist, format);
    while (*format)
    {
        if (*format == '%')
        {
            size_t width = 0;
            bool padding = false;
            switch (*++format)
            {
            case FORMAT_PADDING:
                padding = true;
                format++;
                break;
            }
            while (*format >= '0' && *format <= '9')
            {
                width = (width * 10) + *format - '0';
                format++;
            }
            switch (*format)
            {
            case FORMAT_SIGNED_DECIMAL:;
                int n = va_arg(valist, int);
                if (n < 0)
                {
                    putchar('-');
                    n *= -1;
                }
                puts(itoa((unsigned int)n, 10, width));
                break;
            case FORMAT_UNSIGNED_DECIMAL:
                puts(itoa(va_arg(valist, unsigned int), 10, width));
                break;
            case FORMAT_UNSIGNED_OCTAL:
                puts(itoa(va_arg(valist, unsigned int), 8, width));
            case FORMAT_UNSIGNED_HEX:
                puts(itoa(va_arg(valist, unsigned int), 16, width));
                break;
            case FORMAT_STRING:
                puts(va_arg(valist, const char *));
                break;
            case FORMAT_PERCENT:
                putchar('%');
                break;
            }
        }
        else
        {
            putchar(*format);
        }
        format++;
    }
    va_end(valist);
    return 0;
}

int sprintf(char *str, const char *format, ...)
{
}
#include "stdio.h"
#include <stddef.h>

enum vga_color_t {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

__attribute__ ((packed))
struct cell_t
{
    char c;
    char fg : 4;
    char bg : 4;
};

struct cell_t *screen = (struct cell_t*)0xFF8B8000;
size_t cursor = 0;

const size_t tab_width = 4;
const size_t line_width = 80;

int putchar(int c)
{
    switch(c)
    {
    case '\n':
        cursor += line_width;
        cursor -= cursor % line_width;
        break;
    case '\t':
        cursor += tab_width;
        cursor -= cursor % tab_width;
        break;
    case '\r':
        cursor -= line_width - 1;
        cursor += line_width - (cursor % line_width);
        break;
    default:
        screen[cursor].c = (char) c;
        cursor++;
    }
    return c;
}

int puts(const char *str)
{
    while(*str)
    {
        putchar(*str);
        str++;
    }
    return 0;
}
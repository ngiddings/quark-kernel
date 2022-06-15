#include "platform/putc.h"
#include "mmgr.h"

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

unsigned int cursor = 0;

const unsigned int tab_width = 4;
const unsigned int line_width = 80;
const unsigned int line_count = 25;

struct cell_t screen[4096 / sizeof(struct cell_t)] __attribute__ ((aligned (4096)));

int initialize_screen()
{
    return map_page(screen, 0x000B8000, PAGE_RW);
}

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
    if(cursor >= line_count * line_width)
    {
        for(int i = 0; i < line_count * line_width; i++)
        {
            if(i < (line_count - 1) * line_width)
            {
                screen[i] = screen[i + line_width];
            }
            else
            {
                screen[i].c = ' ';
            }
        }
        cursor -= line_width;
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
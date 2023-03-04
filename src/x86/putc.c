#include "platform/putc.h"
#include "mmgr.h"

#define COM1 0x3f8

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

static inline void outb(short port, char c)
{
    asm volatile(
        "outb %1, %0"
        :: "d" (port), "a" (c)
    );
}

static inline char inb(short port)
{
    char c;
    asm volatile(
        "inb %1, %0"
        : "=r" (c) 
        : "d" (port)
    );
    return c;
}

int initialize_screen()
{
    int status = ENONE;
    if(status = map_page(screen, 0x000B8000, PAGE_RW))
    {
        return status;
    }
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
    outb(COM1 + 4, 0x0F);
    return status;
}

int putchar(int c)
{
    while((inb(COM1 + 5) & 0x20) == 0);
    outb(COM1, c);
    switch(c)
    {
    case '\n':
        cursor += line_width;
        cursor -= cursor % line_width;
        outb(COM1, '\r');
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
#include <stdbool.h>
#include "tty.h"

kernel::TTY::TTY(char* vga)
{
	this->vga = vga;
	this->cursor = 0;
	this->width = 0;
	this->base = 10;
}

kernel::TTY& kernel::TTY::operator<<(kernel::TTY::Format fmt)
{
	switch(fmt)
	{
	case Binary:
		base = 2;
		break;
	case Decimal:
		base = 10;
		break;
	case Hex:
		base = 16;
		break;
	}
}

kernel::TTY& kernel::TTY::operator<<(const char* str)
{
	return printString(str);
}

kernel::TTY& kernel::TTY::operator<<(unsigned int n)
{
	return printNumber(n, base, width);
}

kernel::TTY& kernel::TTY::operator<<(int n)
{
	return printNumber((unsigned int) n, base, width);
}

kernel::TTY& kernel::TTY::operator<<(void* n)
{
    return printNumber((unsigned int) n, 16, 8);
}

kernel::TTY& kernel::TTY::operator<<(char c)
{
        return putChar(c);
}

void kernel::TTY::setWidth(size_t width)
{
	this->width = width;
}

size_t kernel::TTY::getWidth()
{
	return width;
}

void kernel::TTY::clear()
{
	for(int i = 0; i < 80*25; i++)
	{
		vga[i * 2] = ' ';
	}
	cursor = 0;
}

kernel::TTY& kernel::TTY::printNumber(unsigned int n, size_t base,
					size_t width)
{
	const char* digits = "0123456789ABCDEF";
	char str[33];
	size_t i = 1;
	do
	{
		str[i] = digits[n % base];
		n /= base;
		i++;
	} while(n > 0);
	while(i <= width)
	{
		str[i] = '0';
		i++;
	}
	str[0] = '\0';
	for(char* s = str + (i - 1); *s; s--)
	{
		putChar(*s);
	}
	return *this;
}

kernel::TTY& kernel::TTY::printString(const char* str)
{
	while(*str)
	{
		putChar(*str);
		str++;
	}
	return *this;		     
}

kernel::TTY& kernel::TTY::putChar(char c)
{
	switch(c)
	{
	case '\n':
		cursor = (cursor + 80) - (cursor % 80);
		break;
	case '\t':
		cursor = (cursor + 4) - (cursor % 4);
		break;
	case '\r':
		cursor -= cursor % 160;
		break;
	default:
		vga[cursor * 2] = c;
		cursor++;
	}
	return *this;
}

#include <stdbool.h>
#include "tty.hpp"

kernelns::TTY::TTY(char* vga)
{
	this->vga = vga;
	this->cursor = 0;
	this->width = 0;
	this->base = 10;
}

kernelns::TTY& kernelns::TTY::operator<<(kernelns::TTY::Format fmt)
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

kernelns::TTY& kernelns::TTY::operator<<(const char* str)
{
	return printString(str);
}

kernelns::TTY& kernelns::TTY::operator<<(unsigned int n)
{
	return printNumber(n, base, width);
}

kernelns::TTY& kernelns::TTY::operator<<(int n)
{
	return printNumber((unsigned int) n, base, width);
}

kernelns::TTY& kernelns::TTY::operator<<(void* n)
{
    return printNumber((unsigned int) n, 16, 8);
}

kernelns::TTY& kernelns::TTY::operator<<(char c)
{
        return putChar(c);
}

void kernelns::TTY::setWidth(size_t width)
{
	this->width = width;
}

size_t kernelns::TTY::getWidth()
{
	return width;
}

void kernelns::TTY::clear()
{
	for(int i = 0; i < 80*25; i++)
	{
		vga[i * 2] = ' ';
	}
	cursor = 0;
}

kernelns::TTY& kernelns::TTY::printNumber(unsigned int n, size_t base,
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

kernelns::TTY& kernelns::TTY::printString(const char* str)
{
	while(*str)
	{
		putChar(*str);
		str++;
	}
	return *this;		     
}

kernelns::TTY& kernelns::TTY::putChar(char c)
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

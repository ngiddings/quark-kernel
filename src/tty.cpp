#include <stdbool.h>
#include "tty.h"

qkernel::TTY::TTY(char* vga)
{
	this->vga = vga;
	this->cursor = 0;
	this->width = 0;
	this->base = 10;
}

qkernel::TTY& qkernel::TTY::operator<<(qkernel::TTY::Format fmt)
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

qkernel::TTY& qkernel::TTY::operator<<(const char* str)
{
	return printString(str);
}

qkernel::TTY& qkernel::TTY::operator<<(unsigned int n)
{
	return printNumber(n, base, width);
}

qkernel::TTY& qkernel::TTY::operator<<(int n)
{
	return printNumber((unsigned int) n, base, width);
}

qkernel::TTY& qkernel::TTY::operator<<(void* n)
{
    return printNumber((unsigned int) n, 16, 8);
}

qkernel::TTY& qkernel::TTY::operator<<(char c)
{
        return putChar(c);
}

void qkernel::TTY::setWidth(size_t width)
{
	this->width = width;
}

size_t qkernel::TTY::getWidth()
{
	return width;
}

void qkernel::TTY::clear()
{
	for(int i = 0; i < 80*25; i++)
	{
		vga[i * 2] = ' ';
	}
	cursor = 0;
}

qkernel::TTY& qkernel::TTY::printNumber(unsigned int n, size_t base,
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

qkernel::TTY& qkernel::TTY::printString(const char* str)
{
	while(*str)
	{
		putChar(*str);
		str++;
	}
	return *this;		     
}

qkernel::TTY& qkernel::TTY::putChar(char c)
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

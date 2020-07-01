#include <stdbool.h>
#include "tty.h"

qkernel::TTY::TTY(char* vga)
{
	this->vga = vga;
	this->cursor = 0;
}

qkernel::TTY& qkernel::TTY::operator<<(const char* str)
{
	while(*str)
	{
		switch(*str)
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
			vga[cursor * 2] = *str;
			cursor++;
		}
		str++;
	}
	return *this;
}

qkernel::TTY& qkernel::TTY::operator<<(int n)
{
	if(n == 0)
	{
		operator<<('0');
		return *this;
	}

	int sign = n > 0 ? 1 : -1;
	if(sign == -1) operator<<('-');

	int quotient;
	bool showZeros = false;
	for(int divisor = 1000000000; divisor > 0; divisor /= 10)
	{
		quotient = n / divisor;
		char digit = (char) (sign * (quotient % 10) + '0');
		if(digit != '0' || showZeros)
		{
			operator<<(digit);
			showZeros = true;
		}
	}
	return *this;
}

qkernel::TTY& qkernel::TTY::operator<<(char c)
{
	switch(c)
	{
	case '\n':
		cursor = (cursor + 160) - (cursor % 160);
		break;
	case '\t':
		cursor = (cursor + 8) - (cursor % 8);
		break;
	case '\r':
		cursor -= cursor % 160;
		break;
	default:
		vga[cursor * 2] = c;
	}
	cursor++;
	return *this;
}

void qkernel::TTY::clear()
{
	for(int i = 0; i < 80*25; i++)
	{
		vga[i * 2] = ' ';
	}
	cursor = 0;
}

qkernel::TTY& qkernel::TTY::printNumber(unsigned int n, int base = 10, bool sign = true, bool leadingZeros = false)
{
	if(n == 0)
	{
		operator<<('0');
		return *this;
	}

	if(sign)
	{
		int nSig = (int) n;
		int sign = nSig > 0 ? 1 : -1;
		if(sign == -1)
		{
			n = (unsigned int) (nSig * sign);
			operator<<('-');
		}
	}

	int initDivisor = (base == 10) ? 1000000000 :
		((base == 16) ? 0x10000000 :
		 ((base == 8) ? 0x40000000 : 0));
	
	int quotient;
	bool showZeros = leadingZeros;
	for(int divisor = 1000000000; divisor > 0; divisor /= base)
	{
		quotient = n / divisor;
		char digit = (char) (sign * (quotient % base) + '0');
		if(digit != '0' || showZeros)
		{
			operator<<(digit);
			showZeros = true;
		}
	}
	return *this;
}

#ifndef TTY_H_
#define TTY_H_

#include <stddef.h>

namespace qkernel
{

class TTY
{
public:

	enum Format
	{
		Binary,
		Decimal,
		Hex
	};

	TTY(char* vga);

	TTY& operator<<(Format fmt);

	TTY& operator<<(const char* str);

	TTY& operator<<(unsigned int n);
    
	TTY& operator<<(int n);

	TTY& operator<<(void* n);
    
	TTY& operator<<(char c);

	void setWidth(size_t width);

	size_t getWidth();

	void clear();

private:

	TTY& printNumber(unsigned int n, size_t base, size_t width);

	TTY& printString(const char* str);

	TTY& putChar(char c);
    
	char* vga;

	int cursor;

	size_t width;

	size_t base;
	
};
  
}

#endif

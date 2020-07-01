#ifndef TTY_H_
#define TTY_H_

namespace qkernel
{

class TTY
{
public:
  

	TTY(char* vga);

	TTY& operator<<(const char* str);

	TTY& operator<<(int n);
	
	TTY& operator<<(char c);

	void clear();

private:

	TTY& printNumber(unsigned int n, int base, bool sign,
			 bool leadingZeros);
    
	char* vga;

	int cursor;
	
};
  
}

#endif

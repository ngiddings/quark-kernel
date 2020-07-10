#include "inthandlers.hpp"

char* display = (char*) 0xC00B8000;

__attribute__ ((interrupt))
void divisionByZero(void* frame)
{
    *display = 'z';
    display += 2;
}


__attribute__ ((interrupt))
void gpFaultHandler(void* frame, unsigned int error)
{
    *display = 'a';
    asm("hlt");
}

__attribute__ ((interrupt))
void pageFaultHandler(void* frame, unsigned int error)
{
    *display = '0';
    //asm("hlt");
}

__attribute__ ((interrupt))
void doubleFaultHandler(void* frame, unsigned int error)
{
    *display = '#';
    //asm("hlt");
}


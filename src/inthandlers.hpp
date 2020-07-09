#ifndef INTHANDLERS_H
#define INTHANDLERS_H

extern "C"
__attribute__ ((interrupt))
void gpFaultHandler(void* frame, unsigned int error);

extern "C"
__attribute__ ((interrupt))
void pageFaultHandler(void* frame, unsigned int error);

#endif

#ifndef INTHANDLERS_H
#define INTHANDLERS_H

__attribute__ ((interrupt))
void divisionByZero(void* frame);

__attribute__ ((interrupt))
void gpFaultHandler(void* frame, unsigned int error);

__attribute__ ((interrupt))
void pageFaultHandler(void* frame, unsigned int error);

__attribute__ ((interrupt))
void doubleFaultHandler(void* frame, unsigned int error);

__attribute__ ((interrupt))
void syscallHandler(void* frame);

#endif

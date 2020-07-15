#ifndef PIO_H
#define PIO_H

extern "C" void outb(short port, char data);

extern "C" char inb(short port);

#endif
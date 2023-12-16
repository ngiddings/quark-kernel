#ifndef QUARK_MESSAGE_H
#define QUARK_MESSAGE_H

#include <types/pid.h>

typedef struct message_t
{
    pid_t sender;
    unsigned long code;
    unsigned long args[6];
} message_t;

#endif
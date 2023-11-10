#ifndef QUARK_SIGACTION_H
#define QUARK_SIGACTION_H

#include "types/pid.h"

typedef unsigned long (*signal_handler_t)(void*, void*);

struct signal_action_t
{
    pid_t pid;
    signal_handler_t func_ptr;
    void (*trampoline_ptr)();
    void *userdata;
};

#endif
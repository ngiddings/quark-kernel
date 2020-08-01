#ifndef INTERRUPTS_H
#define INTERRUPTS_H

namespace kernel
{
    class Interrupts
    {
    public:

        Interrupts();

        void enable();

        void disable();

    };
}

#endif
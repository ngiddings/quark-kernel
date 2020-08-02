#ifndef INTERRUPTS_H
#define INTERRUPTS_H

namespace kernel
{
    class Interrupts
    {
    public:

        static const unsigned int MAX_SYSCALL_ID = 31;

        Interrupts();

        void enable();

        void disable();

        void addSyscall(unsigned int id, void* function);

    private:

        void* syscalls[MAX_SYSCALL_ID + 1];

    };
}

#endif
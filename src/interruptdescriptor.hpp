#ifndef INTERRUPTDESCRIPTOR_H
#define INTERRUPTDESCRIPTOR_H

#include <stdint.h>

namespace qkernel
{

class InterruptDescriptor
{
public:

    enum Type
    {
	TASK32 = 5,
	TRAP32 = 15,
	INT32 = 14,
	TRAP16 = 7,
	INT16 = 6
    };

    InterruptDescriptor();

    InterruptDescriptor(void* handler, unsigned int type, unsigned int dpl);

    bool present();

    void present(bool present);

    Type type();

    void type(Type type);
    

    void* operator=(void* rhs);
    
private:

    uint16_t m_offset1;
    uint16_t m_selector;
    uint16_t m_zero : 8;
    uint16_t m_type : 4;
    uint16_t m_storage : 1;
    uint16_t m_dpl : 2;
    uint16_t m_present : 1;
    uint16_t m_offset2;
    
};

}

#endif

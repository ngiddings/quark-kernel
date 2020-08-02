#include "../interrupts.hpp"

#include <stdint.h>

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

    InterruptDescriptor(void* handler, Type type, unsigned int dpl);

    bool present();

    void present(bool present);

    Type type();

    void type(Type type);

    unsigned int dpl();

    void dpl(unsigned int dpl);

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

struct IDTInfo
{
    uint16_t size;
    void* location;
};

InterruptDescriptor idt[256];

void lidt()
{
    IDTInfo idtInfo;
    idtInfo.size = sizeof(idt) - 1;
    idtInfo.location = (void*) &idt;
    asm("lidt idt");
}

__attribute__ ((interrupt))
void divisionByZero(void* frame)
{
    
}

__attribute__ ((interrupt))
void gpFaultHandler(void* frame, unsigned int error)
{
    
}

__attribute__ ((interrupt))
void pageFaultHandler(void* frame, unsigned int error)
{
    
}

__attribute__ ((interrupt))
void doubleFaultHandler(void* frame, unsigned int error)
{
    asm("hlt");
}

__attribute__ ((interrupt))
void syscallHandler(void* frame)
{
    
}

InterruptDescriptor::InterruptDescriptor()
{
	this->m_offset1 = 0;
	this->m_selector = 0;
	this->m_zero = 0;
	this->m_type = 0;
	this->m_storage = 0;
	this->m_dpl = 0;
	this->m_present = 0;
	this->m_offset2 = 0;
}

InterruptDescriptor::InterruptDescriptor(void* handler, Type type, unsigned int dpl)
{
	uint32_t offset = (uint32_t) handler;
	this->m_offset1 = (uint16_t) offset;
	this->m_selector = 8;
	this->m_zero = 0;
	this->m_type = (uint16_t) type;
	this->m_storage = 0;
	this->m_dpl = dpl;
	this->m_present = 1;
	this->m_offset2 = offset >> 16;
}

bool InterruptDescriptor::present()
{
	return m_present == 1;
}
void InterruptDescriptor::present(bool present)
{
	m_present = present ? 1 : 0;
}

InterruptDescriptor::Type InterruptDescriptor::type()
{
	return (Type) m_type;
}

void InterruptDescriptor::type(InterruptDescriptor::Type type)
{
	m_type = (unsigned int) type;
}

unsigned int InterruptDescriptor::dpl()
{
	return m_dpl;
}

void InterruptDescriptor::dpl(unsigned int dpl)
{
	m_dpl = dpl;
}

void* InterruptDescriptor::operator=(void* rhs)
{
	uint32_t offset = (uint32_t) rhs;
	m_offset1 = (uint16_t) offset;
	m_offset2 = (offset >> 16);
	return rhs;
}

kernel::Interrupts::Interrupts()
{
    idt[0] = InterruptDescriptor((void*) &divisionByZero, InterruptDescriptor::INT32, 0);
    idt[8] = InterruptDescriptor((void*) &doubleFaultHandler, InterruptDescriptor::INT32, 0);
    idt[0x80] = InterruptDescriptor((void*) &syscallHandler, InterruptDescriptor::INT32, 0);
    // Load interrupt handlers
    // Configure PIC
    lidt();
}

void kernel::Interrupts::enable()
{
    asm("sti");
}

void kernel::Interrupts::disable()
{
    asm("cli");
}
#include "interruptdescriptor.hpp"

kernel::InterruptDescriptor::InterruptDescriptor()
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

kernel::InterruptDescriptor::InterruptDescriptor(void* handler, Type type, unsigned int dpl)
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

bool kernel::InterruptDescriptor::present()
{
	return m_present == 1;
}
void kernel::InterruptDescriptor::present(bool present)
{
	m_present = present ? 1 : 0;
}

kernel::InterruptDescriptor::Type kernel::InterruptDescriptor::type()
{
	return (Type) m_type;
}

void kernel::InterruptDescriptor::type(kernel::InterruptDescriptor::Type type)
{
	m_type = (unsigned int) type;
}

unsigned int kernel::InterruptDescriptor::dpl()
{
	return m_dpl;
}

void kernel::InterruptDescriptor::dpl(unsigned int dpl)
{
	m_dpl = dpl;
}

void* kernel::InterruptDescriptor::operator=(void* rhs)
{
	uint32_t offset = (uint32_t) rhs;
	m_offset1 = (uint16_t) offset;
	m_offset2 = (offset >> 16);
	return rhs;
}

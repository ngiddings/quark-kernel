#include "interruptdescriptor.hpp"

qkernel::InterruptDescriptor::InterruptDescriptor()
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

qkernel::InterruptDescriptor::InterruptDescriptor(void* handler,
						  Type type, unsigned int dpl)
{
	uint32_t offset = (uint32_t) handler;
	this->m_offset1 = (uint16_t) offset;
	this->m_selector = 1;
	this->m_zero = 0;
	this->m_type = (uint16_t) type;
	this->m_storage = 0;
	this->m_dpl = dpl;
	this->m_present = 0;
	this->m_offset = (uint16_t) (offset >> 16);
}

bool qkernel::InterruptDescriptor::present()
{
	return m_present == 1;
}
void qkernel::InterruptDescriptor::present(bool present)
{
	m_present = present ? 1 : 0;
}

Type qkernel::InterruptDescriptor::type()
{

}

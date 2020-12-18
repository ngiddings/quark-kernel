#include "message.hpp"

Message::Message()
{
    m_sender = 0;
    m_type = 0;
    m_args[0] = 0;
    m_args[1] = 0;
    m_args[2] = 0;
}

Message::Message(const Message& copy)
{
    m_sender = copy.m_sender;
    m_type = copy.m_type;
    m_args[0] = copy.m_args[0];
    m_args[1] = copy.m_args[1];
    m_args[2] = copy.m_args[2];
}

Message::Message(unsigned int sender, unsigned int type,
                unsigned long arg1, unsigned long arg2, unsigned long arg3)
{
    m_sender = sender;
    m_type = type;
    m_args[0] = arg1;
    m_args[1] = arg2;
    m_args[2] = arg3;
}

Message& Message::operator=(const Message& rhs)
{
    m_sender = rhs.m_sender;
    m_type = rhs.m_type;
    m_args[0] = rhs.m_args[0];
    m_args[1] = rhs.m_args[1];
    m_args[2] = rhs.m_args[2];
}
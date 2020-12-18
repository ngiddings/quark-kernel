#ifndef MESSAGE_H
#define MESSAGE_H

struct Message
{

    /**
     * @brief Default constructor. Initializes all members to 0.
     * 
     */
    Message();

    /**
     * @brief Copy constructor.
     * 
     * @param copy 
     */
    Message(const Message& copy);

    /**
     * @brief Construct a new Message object.
     * 
     * @param sender 
     * @param type 
     * @param arg1 
     * @param arg2 
     * @param arg3 
     */
    Message(unsigned int sender, unsigned int type, 
            unsigned long arg1, unsigned long arg2, unsigned long arg3);
    
    /**
     * @brief Copy the contents of `rhs` to this object.
     * 
     * @param rhs 
     * @return Message& 
     */
    Message& operator=(const Message& rhs);

    /**
     * @brief PID of the process that generated this message.
     */
    unsigned int m_sender;

    /**
     * @brief Context-specific parameter which indicates to the receiver how
     * the arguments are to be interpreted.
     */
    unsigned int m_type;

    /**
     * @brief Arguments of this message. The meaning of these values depend on
     * context, as well as the values of `m_sender` and `m_type`.
     */
    unsigned long m_args[3];

};

#endif
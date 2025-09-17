#include "Message.h"
Message::Message(const User& user, std::string message)
    :m_user{user}, m_message{message}{}


#pragma once
#include <string>
#include "User.h"

struct Message{
    User m_user;    
    std::string m_message;

    Message(const User& user, std::string message);
};

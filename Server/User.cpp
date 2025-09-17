#include "User.h"
#include <ostream>
#include <string>
#include <iostream>

User::User(int client_fd, std::string username)
    :m_client_fd{client_fd}, m_username{username}{}

User::~User(){
    std::cout << "User destructor called" << std::endl;
}

bool User::operator==(const User& other_user) const {
    return this->m_client_fd == other_user.m_client_fd;
}

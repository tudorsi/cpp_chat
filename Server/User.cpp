#include "User.h"
#include <string>

User::User(int client_fd, std::string username)
    :m_client_fd{client_fd}, m_username{username}{}

bool User::operator==(const User& other_user) const {
    return this->m_client_fd == other_user.m_client_fd;
}

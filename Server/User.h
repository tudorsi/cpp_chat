#pragma once
#include <string>

struct User{
    int m_client_fd;
    std::string m_username;

    User(int client_fd, std::string username);
    ~User();
    bool operator==(const User& other_user) const;
};

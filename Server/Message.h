#pragma once
#include <string>

struct Message{
    int m_client_fd;
    std::string m_message;

    Message(int client_fd, std::string message);
};

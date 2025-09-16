#include "Message.h"
Message::Message(int client_fd, std::string message)
    :m_client_fd{client_fd}, m_message{message}{}

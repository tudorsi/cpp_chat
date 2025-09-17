#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <netinet/in.h>
#include <vector>
#include "Message.h"
#include "User.h"

class Server {

private:
    int m_server_port;
    int m_server_socket;
    sockaddr_in m_server_address;
    std::vector<User*> m_clients;
    std::mutex m_clients_mutex;
    std::queue<Message> m_broadcast_queue;
    std::mutex m_broadcast_mutex;
    std::condition_variable m_broadcast_cv;

public:
    Server(int port);
    void startServer();
    void  handleClientConnection(int client_fd); 
    std::string readFromClient();
    void writeToClient(int client_socket);
    int getServerSocket() const;
    sockaddr_in&  getServerAddress();
    void addMessageToQueue(const Message& message);
    void brodacastMessages();
};

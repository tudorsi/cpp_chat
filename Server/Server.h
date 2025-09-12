#pragma once

#include <mutex>
#include <string>
#include <netinet/in.h>
#include <vector>
class Server {

private:
    int m_server_port;
    int m_server_socket;
    sockaddr_in m_server_address;
    std::vector<int> m_clients; // stores  client file descriptors
    std::mutex clients_mutex;
public:
    Server(int port);
    void startServer();
    void  handleClientConnection(int client_fd); 
    std::string readFromClient();
    void writeToClient(int client_socket);
    int getServerSocket() const;
    sockaddr_in&  getServerAddress();
};

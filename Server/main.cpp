#include "Server.h"
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <ostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>

int main(){
    std::unique_ptr<Server> server = std::make_unique<Server>(8080);
    std::unique_ptr<sockaddr_in> client_addr = std::make_unique<sockaddr_in>();
    int client_fd;
    socklen_t client_addr_len = sizeof(*client_addr);

    try{
        server->startServer();
    }catch(std::runtime_error e){
        std::cerr << e.what();
        return -1;
    }
    
    std::thread broadcast_thread([&server]{
        while(true){
            server->brodacastMessages();
        }
    });

    broadcast_thread.detach();

   
    while(true){
        client_fd = accept(server->getServerSocket(),reinterpret_cast<sockaddr*>(client_addr.get()), &client_addr_len);
        
        std::thread client_thread([&server, client_fd](){
            try{
                server->handleClientConnection(client_fd);
            }catch(std::runtime_error e){
                std::cerr << "Client thread threw an exception: " << e.what() << std::endl;
            }
        });
        client_thread.detach();
    }

    return 0;
}

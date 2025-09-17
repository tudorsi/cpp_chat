#include "Server.h"
#include <iostream>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include<unistd.h>
#include <ostream>
#include <stdexcept>
#include <sys/socket.h>
#include <vector>

Server::Server(int port){
    m_server_port = port;
    m_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    m_server_address = sockaddr_in();

    m_server_address.sin_family = AF_INET;
    m_server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    m_server_address.sin_port = htons(m_server_port);
    
}

void Server::startServer(){
    if(bind(m_server_socket, (sockaddr*) &m_server_address, (socklen_t) sizeof(m_server_address)) < 0){
        throw std::runtime_error("Failed to bind address to server!");
    }else{
        std::cout << "Address bound to server!" << std::endl;
    }

    if(listen(m_server_socket, 10) < 0){
        throw std::runtime_error("Failed to start listening for connections");
    }else{
        std::cout << "Listening for incomming connections..." << std::endl;
    }
}

void Server::handleClientConnection(int client_fd){ 
    if(client_fd < 0){
        std::cout << "client socket is incorrect!" << std::endl;
        throw std::runtime_error("Cannot accept client connection! ");
    }else{
        std::cout << "Client connection requested!" << std::endl;
        
        //Read client name
        std::vector<char> read_buffer(2048);
        ssize_t buf_size = 0;
        std::string client_message;
        buf_size = read(client_fd, read_buffer.data(), read_buffer.size());
        client_message = std::string(read_buffer.data(), buf_size);

        if(buf_size < 0){
            throw std::runtime_error("Failed to read client message!");
        }else if(buf_size == 0){
            std::cout << "Client disconnected!" << std::endl;
        }else{
            read_buffer.push_back('\0');
            m_clients_mutex.lock();
            User user{client_fd, client_message};
            m_clients.push_back(user);
            m_clients_mutex.unlock();   
            std::cout << "Client added to vector" << std::endl;

            std::fill(read_buffer.begin(), read_buffer.end(), 0);
            client_message = "";
            
            //read messages while client is connected
            //TODO: extract message reading to separate method
            while(true){
                buf_size = read(client_fd, read_buffer.data(), read_buffer.size());
                if(buf_size < 0){
                    throw std::runtime_error("Failed to read client message!");
                }else if(buf_size == 0){
                    //remove the disconnected client
                    m_clients_mutex.lock();
                    m_clients.erase(std::remove(m_clients.begin(), m_clients.end(), user), m_clients.end());
                    m_clients_mutex.unlock();

                    std::cout << "Client disconnected!" << std::endl;
                    break;
                }

                read_buffer.push_back('\n');
                client_message = std::string(read_buffer.begin(), read_buffer.end());
                std::unique_ptr<Message> message = std::make_unique<Message>(user, client_message); //TODO: store message on stack
                std::cout << user.m_username << " said: " << client_message;
                addMessageToQueue(*message);

                std::fill(read_buffer.begin(), read_buffer.end(), 0); // clear the buffer before next read
                client_message = "";
            }
        }
   }
}

int Server::getServerSocket() const{
    return m_server_socket;
}

sockaddr_in& Server::getServerAddress(){
    return m_server_address;
}

void Server::addMessageToQueue(const Message& message){
    std::lock_guard<std::mutex> lock(m_broadcast_mutex);
    m_broadcast_queue.push(message);
    m_broadcast_cv.notify_one();
    std::cout << "message added to queue!" << std::endl;
}

void Server::brodacastMessages(){
    std::unique_lock<std::mutex> messages_lock(m_broadcast_mutex);
    m_broadcast_cv.wait(messages_lock, [this]{return !m_broadcast_queue.empty();});
    
    Message message = m_broadcast_queue.front();
    std::cout << "next message in queue is: " << message.m_message << std::endl;
    std::string message_to_send = message.m_user.m_username + ": " + message.m_message;

    for(const auto& client : m_clients){
       if(client.m_client_fd!= message.m_user.m_client_fd){
            send(client.m_client_fd, message_to_send.c_str(), std::strlen(message_to_send.c_str()), 0);
            std::cout << "Message broadcasted to clients" << std::endl;
       } 
    }
    m_broadcast_queue.pop();
}

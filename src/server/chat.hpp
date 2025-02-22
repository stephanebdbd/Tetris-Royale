#ifndef SERVER_CHAT_HPP
#define SERVER_CHAT_HPP

#include "io.hpp"
#include "../common/json.hpp"
#include <unordered_map>
#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

class ServerChat {

private:
    std::unordered_map<std::string, int> clients; //  socket-> pseudoName
    std::mutex clientsMutex;

public:
    ServerChat()   = default;
    ~ServerChat()  = default;
    void start();
    void stop();
        
    //thread pour gerer un chat d'un client
    void processClientChat(int clientSocket);
    //envoi d'un message à tous les clients
    void broadcastMessage(const std::string& message, const std::string& channel);
    //envoi d'un message à un client
    void sendMessage(int clientSocket, const std::string& message);
};

#endif // SERVER_CHAT_HPP
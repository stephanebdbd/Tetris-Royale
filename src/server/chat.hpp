#pragma once
#include "ChatMessage.hpp"
#include "io.hpp"
#include <unordered_map>
#include <mutex>
#include <thread>
#include <vector>
#include <string>

class ServerChat {
public:
    ServerChat()   = default;
    ~ServerChat()  = default;
    void start();
    void stop();
    
private:
    int serverSocket;
    std::unordered_map<std::string, int> clients; // userId -> socket
    std::mutex clientsMutex;

    //thread pour gerer un chat d'un client
    void handleClientChat(int clientSocket);
    //envoi d'un message à tous les clients
    void broadcastMessage(const ChatMessage& msg, const std::string& channel);
    //envoi d'un message à un client
    void sendMessage(int clientSocket, const std::string& message);
};

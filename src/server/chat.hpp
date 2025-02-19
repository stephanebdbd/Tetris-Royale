#ifndef SERVER_CHAT_HPP
#define SERVER_CHAT_HPP

#include "ChatMessage.hpp"
#include "io.hpp"
#include <unordered_map>
#include <mutex>
#include <thread>
#include <vector>
#include <string>

class ServerChat {

private:
    std::unordered_map<std::string, int> clients; // pseudoName -> socket
    std::mutex clientsMutex;

public:
    ServerChat()   = default;
    ~ServerChat()  = default;
    void start();
    void stop();
        
    //thread pour gerer un chat d'un client
    void processClientChat(int clientSocket);
    //envoi d'un message à tous les clients
    void broadcastMessage(const ChatMessage& msg, const std::string& channel);
    //envoi d'un message à un client
    void sendMessage(int clientSocket, const std::string& message);
};

#endif // SERVER_CHAT_HPP

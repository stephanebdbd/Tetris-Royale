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
    std::unordered_map<int, std::string> clients; // clientSocket -> pseudoName
    std::mutex clientsMutex;

public:
    ServerChat() = default;
    ~ServerChat() = default;

    // Supprimez le constructeur de copie et l'opérateur d'affectation de copie
    ServerChat(const ServerChat&) = delete;
    ServerChat& operator=(const ServerChat&) = delete;

    void start();
    void stop();
        
    // thread pour gérer un chat d'un client
    void processClientChat(int clientSocket);
    // envoi d'un message à tous les clients
    void broadcastMessage(const std::string& message);
    // envoi d'un message à un client
    void sendMessage(int clientSocket, const std::string& message);
    // obtenir le menu de chat
    std::string getChatMenu() const;
};

#endif // SERVER_CHAT_HPP
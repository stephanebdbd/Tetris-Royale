#ifndef SERVER_CHAT_HPP
#define SERVER_CHAT_HPP

#include "../common/json.hpp"

#include <unordered_map>
#include <thread>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <atomic>

class Server;
enum class MenuState;

class ServerChat {
private:
    static bool messagesWaitForDisplay;

public:
    ServerChat() = default;
    ~ServerChat() = default;

    // Supprimez le constructeur de copie et l'opérateur d'affectation de copie
    ServerChat(const ServerChat&) = delete;
    ServerChat& operator=(const ServerChat&) = delete;
        
    // thread pour gérer un chat d'un client
    void processClientChat(int clientSocket, int clientId, Server &server, MenuState state, std::string menu);
    
    // envoi d'un message à un client
    void sendMessage(int clientSocket, std::string sender, const std::string& message);
    
    // memoire pour stocker les messages si le client n'est pas en train de chatter
    static bool initMessageMemory(const std::string& filename);
    static bool saveMessage(const std::string& filename, const std::string& message);
    static bool FlushMemory(const std::string& filename);
};

#endif // SERVER_CHAT_HPP
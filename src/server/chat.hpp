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
    // obtenir le menu de chat
    std::string getChatMenu() const;
};

#endif // SERVER_CHAT_HPP
#ifndef SERVER_CHAT_HPP
#define SERVER_CHAT_HPP

#include "chatRoom.hpp"
#include "../common/json.hpp"

#include <unordered_map>
#include <thread>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <vector>



class ServerChat {
private:
    std::vector<chatRoom> chatRooms;  // Liste des salles de chat

public:
    ServerChat() = default;
    ~ServerChat() = default;

    // Supprimez le constructeur de copie et l'opérateur d'affectation de copie
    ServerChat(const ServerChat&) = delete;
    ServerChat& operator=(const ServerChat&) = delete;
        
    // thread pour gérer un chat d'un client
    void processClientChat(int clientSocket, std::unordered_map<std::string, int>& pseudoSocket);
    // envoi d'un message à un client
    void sendMessage(int clientSocket, std::string sender, const std::string& message);
    // obtenir le menu de chat
    std::string getChatMenu() const;
    // obtenir le statut du chat
    bool getIsChatActive() const;
};

#endif // SERVER_CHAT_HPP
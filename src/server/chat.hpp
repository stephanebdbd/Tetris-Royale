#ifndef SERVER_CHAT_HPP
#define SERVER_CHAT_HPP

#include "../common/json.hpp"
#include "Data.hpp"

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

class ServerChat{
private:
    bool messagesWaitForDisplay = false;

public:
    ServerChat() = default;
    ~ServerChat() = default;

    // Supprimez le constructeur de copie et l'opérateur d'affectation de copie
    ServerChat(const ServerChat&) = delete;
    ServerChat& operator=(const ServerChat&) = delete;
        
    // thread pour gérer un chat d'un client
    void processClientChat(int clientSocket, int clientId, Server &server, MenuState state, std::string menu);
    
    // envoi d'un message à un client
    void sendMessage(int clientSocket, std::string sender, const std::string& message, bool isOnline);
    
    // memoire pour stocker les messages si le client n'est pas en train de chatter
    json openFile(const std::string& filename);
    void writeFile(const std::string& filename, const json& j);
    bool initMessageMemory(const std::string& filename);
    void saveMessage(const std::string& filename, const std::string& message);
    void FlushMemory(const std::string& filename, Server &server);

    //getter des données du fichier json du client
    std::vector<std::string> getMyFriends(const std::string& pseudo);
    std::vector<std::string> getMyFriendRequests(const std::string& pseudo);
    std::vector<std::string> getMyRooms(const std::string& pseudo);
    std::vector<std::string> getMyRoomRequests(const std::string& pseudo);
};

#endif // SERVER_CHAT_HPP
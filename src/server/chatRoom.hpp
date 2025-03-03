#ifndef CHATROOM_HPP
#define CHATROOM_HPP

#include "chat.hpp"
#include <unordered_map>
#include <set>
#include <mutex>
#include <nlohmann/json.hpp>


class ServerChat;
class Server;

class chatRoom {
private:
    
    std::set<std::string> clients;                        // pseudo des clients dans la salle
    std::set<std::string> receivedReq;                    // pseudo des demandes reçues
    std::set<std::string> sentReq;                        // pseudo des demandes envoyées
    std::mutex clientsMutex;                              // Mutex pour les clients
    std::mutex requestsMutex;                             // Mutex pour les demandes

public:
    chatRoom();  // Constructeur
    ~chatRoom() = default;                                   // Destructeur

    void addClient(const std::string& pseudo);                                                             // Ajouter un client
    void removeClient(const std::string& pseudo);                                                          // Supprimer un client
    void sendClientRequest(const std::string& pseudo, const std::string& message);                     // Envoi d'une demande de chat
    void acceptClientRequest(const std::string& pseudo);                                                   // Accepter une demande de chat                                     
    void refuseClientRequest(const std::string& pseudo);                                                   // Refuser une demande de chat
    void broadcastMessage(const std::string& message, const std::string& sender, Server* server);          // Diffuser message
};

#endif // chatRoom_HPP
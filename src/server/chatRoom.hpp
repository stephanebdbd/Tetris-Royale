#ifndef CHATROOM_HPP
#define CHATROOM_HPP

#include "chat.hpp"
#include <unordered_map>
#include <vector>
#include <mutex>
#include <nlohmann/json.hpp>


class chatRoom {
private:
    int RoomID;
    std::vector<int> clients;                             // ID des clients
    std::mutex clientsMutex;                              // Mutex pour les clients
    int adminID;                                          // ID de l'administrateur
    std::unordered_map<int, std::string> clientRequests;  // Demande de chat


public:
    chatRoom();

    void addClient(int id);  // Ajouter un client
    void removeClient(int id);  // Supprimer un client
    void sendClientRequest(const std::string& pseudoName, const std::string& message);  // Envoi d'une demande de chat
    void acceptClientRequest(int id);  // Accepter une demande

    void handleClient(int clientSocket);  // Gérer un client (écoute messages)
    void broadcastMessage(const std::string& message, const std::string& sender);  // Diffuser message

    std::string getChatMenu() const;  // Obtenir le menu de chat
};

#endif // chatRoom_HPP
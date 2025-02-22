#ifndef CHATMANAGER_HPP
#define CHATMANAGER_HPP

#include "chat.hpp"
#include "User.hpp"
#include "../include.hpp"
#include <unordered_map>
#include <vector>
#include <mutex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ChatManager {
private:
    ServerChat chat;
    std::unordered_map<std::string, int> clients;  // Associe pseudo -> socket
    std::vector<User> usersRequest;  // Liste des utilisateurs en attente
    std::mutex clientsMutex;  // Mutex pour la gestion concurrente

public:
    ChatManager(ServerChat chat);
    void start();  // Démarrer le chat
    void stop();  // Arrêter le chat

    void addClient(const std::string& pseudoName, int socket);  // Ajouter un client
    void removeClient(const std::string& pseudoName);  // Supprimer un client
    void sendClientRequest(const std::string& pseudoName, const std::string& message);  // Envoi d'une demande de chat
    void acceptClientRequest(const std::string& pseudoName, int socket);  // Accepter une demande

    void handleClient(int clientSocket);  // Gérer un client (écoute messages)
    void broadcastMessage(const std::string& message, const std::string& sender);  // Diffuser message

    std::string getChatMenu() const;  // Obtenir le menu de chat
};

#endif // CHATMANAGER_HPP
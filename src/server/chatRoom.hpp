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
    std::string filename ;                                // Nom du fichier
    std::string roomName;                                 // Nom de la salle
    int adminId;                                          // Admin de la salle
    std::set<std::string> clients;                        // pseudo des clients dans la salle
    std::set<std::string> receivedReq;                    // pseudo des demandes reçues
    std::set<std::string> sentReq;                        // pseudo des demandes envoyées
    std::mutex clientsMutex;                              // Mutex pour les clients
    std::mutex requestsMutex;                             // Mutex pour les demandes

public:
    chatRoom(std::string room_name, int admin_id);           // Constructeur
    ~chatRoom() = default;                                   // Destructeur

    void init_chatRoom();                                        // Initialiser la salle
    void addClient(const std::string& pseudo);                                                             // Ajouter un client
    void removeClient(const std::string& pseudo);                                                          // Supprimer un client
    void sendClientRequest(const std::string& pseudo, const std::string& message);                     // Envoi d'une demande de chat
    void acceptClientRequest(const std::string& pseudo);                                                   // Accepter une demande de chat                                     
    void refuseClientRequest(const std::string& pseudo);                                                   // Refuser une demande de chat
    void broadcastMessage(const std::string& message, const std::string& sender, Server& server);          // Diffuser message
    //getters 
    std::string getRoomName() const;                                                                        // Obtenir le nom de la salle
    int getAdminId() const;                                                                                 // Obtenir l'ID de l'admin
    std::set<std::string> getClients() const;                                                               // Obtenir les clients
    std::set<std::string> getReceivedReq() const;                                                           // Obtenir les demandes reçues
    std::set<std::string> getSentReq() const;                                                               // Obtenir les demandes envoyées                                                                     // Définir l'ID de l'admin
};

#endif // chatRoom_HPP
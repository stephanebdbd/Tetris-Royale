#ifndef CHATROOM_HPP
#define CHATROOM_HPP

#include <unordered_map>
#include <vector>
#include <mutex>
#include <algorithm>
#include <fstream>

#include "../common/json.hpp"
#include "Data.hpp"



class ServerChat;
class Server;

class chatRoom : public Data {
private:
    std::string filename;                                 // Nom du fichier
    std::string roomName;                                 // Nom de la salle
    std::string adminPseudo;                              // Pseudo de l'admin
    std::mutex clientsMutex;                              // Mutex pour les clients
    std::mutex requestsMutex;                             // Mutex pour les demandes

public:
    chatRoom() = default;                                                // Constructeur par défaut
    chatRoom(std::string room_name, std::string admin_pseudo);           // Constructeur
    ~chatRoom() = default;                                               // Destructeur

    void init_chatRoom();                                               // Initialiser la salle
    //bool isInKey(const std::string& key , const std::string& pseudo) const; // Vérifier si un client est dans la salle
    bool isClient(const std::string& pseudo) const;                     // Vérifier si un client est dans la salle
    bool isAdmin(const std::string& pseudo) const;                      // Vérifier si un client est admin
    bool isInReceivedReq(const std::string& pseudo) const;              // Vérifier si une demande est reçue
    void addAdmin(const std::string& pseudo);                           // Ajouter un admin
    void removeAdmin(const std::string& pseudo);                        // Supprimer un admin
    void addClient(const std::string& pseudo);                          // Ajouter un client
    void removeClient(const std::string& pseudo);                       // Supprimer un client
    void addReceivedRequest(const std::string& pseudo);                 // Ajouter une demande reçue
    //a faire  ##################################################
    //void sendRequestToClient(const std::string& pseudo, const std::string& message);                     // Envoi d'une demande de chat                           
    //############################################################
    void acceptClientRequest(const std::string& pseudo);                                                  // Accepter une demande de chat
    void refuseClientRequest(const std::string& pseudo);                                                   // Refuser une demande de chat
    // Diffuser message                                                   
    void broadcastMessage(const std::string& message, const std::string& sender, Server& server);
    //supprimer la room
    void deleteRoomFile();
        
    //getters 
    std::string getRoomName() const;                                                                        // Obtenir le nom de la salle
    std::vector<std::string> getadminPseudo() const;                                                                                 // Obtenir l'ID de l'admin
    std::vector<std::string> getClients() const;                                                               // Obtenir les clients
    std::vector<std::string> getReceivedReq() const;                                                           // Obtenir les demandes reçues
    std::vector<std::string> getSentReq() const;                                                               // Obtenir les demandes envoyées
};

#endif // chatRoom_HPP
#ifndef CHATROOM_HPP
#define CHATROOM_HPP

#include <unordered_map>
#include <set>
#include <mutex>
#include <algorithm>
#include <fstream>

#include "../common/json.hpp"



class ServerChat;
class Server;

class chatRoom {
    std::string filename;                                // Nom du fichier
    std::string roomName;                                 // Nom de la salle
    std::string adminPseudo;                                          // ID de l'admin
    std::mutex clientsMutex;                              // Mutex pour les clients
    std::mutex requestsMutex;                             // Mutex pour les demandes

    public:
        chatRoom(std::string room_name, std::string admin_pseudo);           // Constructeur
        ~chatRoom() = default;                                               // Destructeur

        void init_chatRoom();                                               // Initialiser la salle
        bool isInKey(const std::string& key , const std::string& pseudo) const; // Vérifier si un client est dans la salle
        bool isClient(const std::string& pseudo) const;                     // Vérifier si un client est dans la salle
        bool isAdmin(const std::string& pseudo) const;                      // Vérifier si un client est admin
        void addAdmin(const std::string& pseudo);                           // Ajouter un admin
        void addClient(const std::string& pseudo);                          // Ajouter un client
        void removeClient(const std::string& pseudo);                       // Supprimer un client
        void addReceivedRequest(const std::string& pseudo);                 // Ajouter une demande reçue
        //a faire  ##################################################
        void sendRequestToClient(const std::string& pseudo, const std::string& message);                     // Envoi d'une demande de chat                           
        //############################################################
        void refuseClientRequest(const std::string& pseudo);                                                   // Refuser une demande de chat
        void broadcastMessage(const std::string& message, const std::string& sender, Server& server);          // Diffuser message
        //getters 
        std::string getRoomName() const;                                                                        // Obtenir le nom de la salle
        std::set<std::string> getadminPseudo() const;                                                                                 // Obtenir l'ID de l'admin
        std::set<std::string> getClients() const;                                                               // Obtenir les clients
        std::set<std::string> getReceivedReq() const;                                                           // Obtenir les demandes reçues
        std::set<std::string> getSentReq() const;                                                               // Obtenir les demandes envoyées                                                                     // Définir l'ID de l'admin
        // saveData
        void saveData(const std::string& filename, const std::string& key, const std::string& value);
        // loadData
        std::set<std::string> loadData(const std::string& filename, const std::string& key) const;
        // deleteData
        void deleteData(const std::string& filename, const std::string& key, const std::string& value);
};

#endif // chatRoom_HPP
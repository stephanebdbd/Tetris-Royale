#pragma once

#include <sqlite3.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <thread>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <atomic>   
// #include <ctime>
#include <iostream>
#include "../../common/json.hpp"
#include <memory>
#include <regex>
#include <sodium.h>
#include"database.hpp"

class ChatRoom {
    private:
        std::shared_ptr<DataBase> db; // Base de données partagée pour interagir avec le serveur
    
    public:
        // Constructeur avec un pointeur partagé sur la base de données
        ChatRoom(std::shared_ptr<DataBase> db){ this->db = db;}
        //bool deleteRoom();   
        // Initialiser la chat room
         // Créer une salle de discussion
        bool createRoom(const std::string &room_name, const std::string &admin_pseudo);

        // Vérifie si l'utilisateur est admin dans la salle spécifiée
        bool isAdmin(const std::string& pseudo, const std::string& room_name) const;

        // Vérifie si l'utilisateur est un client dans la salle spécifiée
        bool isClient(const std::string& pseudo, const std::string& room_name) const;

        // Vérifie si l'utilisateur a une demande en attente dans la salle spécifiée
        bool isPendingReq(const std::string& pseudo, const std::string& room_name) const;

        // Ajouter un administrateur à une salle de discussion
        bool addAdmin(const std::string& target_pseudo, const std::string& room_name);

        // Retirer un administrateur d'une salle de discussion
        bool removeAdmin( const std::string& client_pseudo, const std::string& room_name);

        // Ajouter un client à une salle de discussion
        bool addClient( const std::string& client_pseudo, const std::string& room_name);

        // Retirer un client d'une salle de discussion
        bool removeClient( const std::string& client_pseudo, const std::string& room_name);

        void refuseClientRequest( const std::string& client_pseudo, const std::string& room_name) ;

        void acceptClientRequest( const std::string& client_pseudo, const std::string& room_name) ;
        bool checkroomExist(const std::string& room_name) const; // Vérifier si la salle existe
        std::vector<std::string> getChatRoomsForUser(const std::string& username) const ;

        void joinRoom(const std::string& pseudo, const std::string& room_name) ;
        std::vector<std::string> getMembers(const std::string& room_name) const ;
        std::vector<std::string> getAdmins(const std::string& room_name) ;
        std::vector<std::string> getClientPending(const std::string& room_name) ;
        bool deleteChatRoom(const std::string& room_name) ;
        bool quitRoom(const std::string& room_name, const std::string& username) ;
        std::vector<std::string> getChatRooms() const;
        
        bool saveMessageToRoom(const std::string& sender, const std::string& room_name, const std::string& msg) ;
        std::string getMessagesFromRoom(const std::string& room_name) ;
        void processRoomChat(int senderSocket, const std::string& sender, const std::string& roomName, std::shared_ptr<std::vector<int>> roomSockets) ;



};

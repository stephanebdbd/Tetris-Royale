#pragma once


#include "../../common/json.hpp"

#include <unordered_map>
#include <thread>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <atomic>
#include "Database.hpp"

class Chat{
private:
    std::shared_ptr<DataBase> db;  // Instance de la classe Chat pour interagir avec la base de données
    bool messagesWaitForDisplay = false;




public:
    Chat(std::shared_ptr<DataBase> db){ this->db = db;}
    //void processRoomChat(int senderSocket, const std::string& sender, const std::string& roomName, std::shared_ptr<std::vector<int>> roomSockets) ;

    bool saveMessage(const std::string& sender, const std::string& receiver, const std::string& content);
    std::string getMsgBetweenUsers(const std::string &id_user, const std::string &id_friend);
    QueryResult getAllUserMsg(const std::string &id_user);
        
    // thread pour gérer un chat d'un client
    bool processClientChat(int senderSocket, const std::string& sender, const std::map<std::string, int>& receiver, json& msg) ;

    // envoi d'un message à un client
    void sendMessage(json& msg, const std::map<std::string, int>& receiver) ;

    // envoi des anciennes conversations
    void sendOldMessages(int senderSocket, const std::string& sender, const std::string& receiver) ;
    
    //getter des données du fichier json du client
    std::vector<std::string> getMyRooms(const std::string& pseudo);
};





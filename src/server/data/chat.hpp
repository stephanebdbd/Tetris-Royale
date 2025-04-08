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
#include "database.hpp"

class Chat{
private:
    std::shared_ptr<DataBase> db;  // Instance de la classe Chat pour interagir avec la base de données
    bool messagesWaitForDisplay = false;




public:
    Chat(std::shared_ptr<DataBase> db){ this->db = db;}

    bool saveMessage(const std::string& sender, const std::string& receiver, const std::string& content);
    std::string getTime();
    std::string getMsgBetweenUsers(const std::string &id_user, const std::string &id_friend);
    QueryResult getAllUserMsg(const std::string &id_user);
        
    // thread pour gérer un chat d'un client
    void processClientChat(int clientSocket, int clientId, std::string sender);
    
    // envoi d'un message à un client
    void sendMessage(int clientSocket, std::string sender, const std::string& message, bool isOnline);
    
    //getter des données du fichier json du client
    std::vector<std::string> getMyRooms(const std::string& pseudo);
};





/*#pragma once

#include <sqlite3.h>
#include <fstream>
#include <sstream>
// #include <ctime>
#include <iostream>
#include <memory>
#include <regex>
#include <sodium.h>
#include"database.hpp"

#include "../../common/json.hpp"

class Chat {
private:
    std::shared_ptr<DataBase> db;

public:
    explicit Chat(std::shared_ptr<DataBase> db){ this->db = db;}

    bool saveMessage(const std::string& sender, const std::string& receiver, const std::string& content);
    std::string getTime();

    std::string getMsgBetweenUsers(const std::string &id_user, const std::string &id_friend);

    QueryResult getAllUserMsg(const std::string &id_user);

};
*/
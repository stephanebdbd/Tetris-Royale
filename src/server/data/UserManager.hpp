#pragma once

#include <sqlite3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <regex>
#include <sodium.h>


#include "Database.hpp"


class DataManager
{
private:

    // Pointer to the database object
    std::shared_ptr<DataBase> db;

public:

    DataManager(std::shared_ptr<DataBase> db){ this->db = db;}
    bool checkPwd(const std::string &id_user, const std::string &pwd);
    
    bool loginUser(const std::string &username, const std::string &pwd);
    bool registerUser(const std::string &username, const std::string &pwd);
  

    QueryResult getUsername(const std::string &id_user);
    QueryResult getUserId(const std::string& username);

    QueryResult updateUserName(const std::string &username, const std::string &pwd, const std::string &new_username);
    
    QueryResult updatePwd(const std::string &username, const std::string &pwd, const std::string &new_pwd);
    
    bool deleteFriend(const std::string &username, const std::string &friend_username);
    
    QueryResult deleteAccount(const std::string &id_user, const std::string &pwd);
    
    QueryResult getUserFriends(const std::string &id_user);
    bool authenticateUser(const std::string& username, const std::string& password) ;

    bool addGameState(const std::string &id_player1, const std::string &id_player2,const std::string &id_session, const std::string &game_state);
    
    QueryResult getGameStates(const std::string &id_session);
    
    QueryResult getSessionId(const std::string &id_player);
    
    QueryResult deleteGameStates(const std::string &id_session);
    
    

    // Fonction pour envoyer une demande d'ami
    bool sendFriendRequest(const std::string& sender, const std::string& receiver);

    // Fonction pour accepter une demande d'ami
    bool acceptFriendRequest(const std::string& receiver, const std::string& sender);

    // Fonction pour rejeter une demande d'ami
    bool rejectFriendRequest(const std::string& receiver, const std::string& sender);

    // Fonction pour vérifier si deux utilisateurs sont amis
    bool areFriends(const std::string& user1, const std::string& user2);
    //getRequestList
    // Fonction pour récupérer la liste des amis d'un utilisateur
    std::vector<std::string> getFriendList(const std::string& user);

    // Fonction pour récupérer la liste des demandes d'amis d'un utilisateur
    std::vector<std::string> getRequestList(const std::string& user) ;
    bool userExists(const std::string& username) const;
    bool userNotExists(const std::string& username) const;
    bool hasSentRequest(const std::string& sender, const std::string& receiver) const;

    std::vector<std::string> getList(const std::string& user, const std::string& status);
    //bool areFriends(const std::string& user1, const std::string& user2);  // Vérifier si deux utilisateurs sont amis
    std::vector<std::pair<std::string, int>> getRanking() const;  // Récupérer le classement des utilisateurs


};

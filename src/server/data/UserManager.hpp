#pragma once

#include <sqlite3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <regex>
#include <sodium.h>
#include "Database.hpp"


class DataManager{
    // Pointer -> instance databse
    std::shared_ptr<DataBase> db;
    std::vector<std::string> getUserListByStatus(const std::string& user, const std::string& status);
    QueryResult getDataFromTable(
        const std::string& table,
        const std::string& columns,
        const std::string& joinTables = "",
        const std::string& joinConditions = "",
        const std::string& whereConditions = "");

    public:

        DataManager(std::shared_ptr<DataBase> db){ this->db = db;}
        
        //gerer les clients du serveur
        bool userExists(const std::string& username) const;
        bool userNotExists(const std::string& username) const;
        bool checkPwd(const std::string &id_user, const std::string &pwd);
        bool loginUser(const std::string &username, const std::string &pwd);
        bool registerUser(const std::string &username, const std::string &pwd, const int& avatrId = -1);
        QueryResult getUsername(const std::string &id_user);
        QueryResult getUserId(const std::string& username);
        int getUserAvatarId(const std::string& username);
        QueryResult updateUserName(const std::string &username, const std::string &pwd, const std::string &new_username);
        QueryResult updatePwd(const std::string &username, const std::string &pwd, const std::string &new_pwd);
        bool authenticateUser(const std::string& username, const std::string& password) ;
        
        
        // gerer les demandes d amis
        bool sendFriendRequest(const std::string& sender, const std::string& receiver);
        bool acceptFriendRequest(const std::string& receiver, const std::string& sender);
        bool rejectFriendRequest(const std::string& receiver, const std::string& sender);
        std::vector<std::string> getRequestList(const std::string& user) ;
        
        // gerer la liste des amis
        bool areFriends(const std::string& user1, const std::string& user2);
        bool deleteFriend(const std::string &username, const std::string &friend_username);
        std::vector<std::string> getFriendList(const std::string& user);
        
        //gerer les invitation aux games des users
        bool sendInvitationToFriend(const int& gameRoom, const std::string& sender, const std::string& player, const std::string& invitation_type);
        bool acceptGameInvitation(const int& gameRoom, const std::string& player);
        std::vector<std::vector<std::string>> getListGameRequest(const std::string& player);
        
        //gerer le score du user
        QueryResult updateHighScore(const std::string& username, const int& bestScore);
        std::map<std::string, std::vector<std::string>> getRanking() const;
        std::pair<std::string, std::string> getCurrentPlayerInfo(const std::string& username) const;
};

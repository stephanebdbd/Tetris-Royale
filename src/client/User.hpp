#pragma once

#include <string>
#include "FriendList.hpp"

class User {
private:
    std::string userName;
    std::string password;
    int highScore = 0;
    int matricule;
    FriendList* friendList;
    bool isOnline = false;
public:
    User(const std::string& userName, const std::string& password, int matricule = 0);
    
    void addFriend(User* friendUser);
    bool login(const std::string& passwordInput);
    void logout();
    
    void setHighScore(int newHighScore);
    
    int getHighScore() const;
    bool getIsOnline() const;
    std::string getUserName() const;
    int getMatricule() const;
    FriendList& getFriendList();
};

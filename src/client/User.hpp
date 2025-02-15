#pragma once
#include <string>
#include "FriendList.hpp"

class User {
private:
    std::string userName;     // Nom d'utilisateur
    std::string pseudonym;    // Pseudonyme
    std::string password;
    int highScore;
    bool isOnline;
    FriendList friendList;    // Liste des amis

public:
    User(const std::string& userName, const std::string& pseudonym, const std::string& password);

    std::string getUserName() const;
    std::string getPseudonym() const;
    void setPseudonym(const std::string& newPseudonym);

    bool login(const std::string& password);
    void logout();
    bool getIsOnline() const;
    void setHighScore(int score);
    int getHighScore() const;

    // Méthodes pour gérer les amis
    void addFriend(User* newFriend);
    void removeFriend(User* friendToRemove);
    bool isFriend(User* friendToCheck) const;
    const std::vector<User*>& getFriends() const;
    size_t getNumberOfFriends() const;
};


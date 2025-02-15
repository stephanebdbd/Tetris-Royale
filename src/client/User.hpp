#pragma once
#include <string>
#include "FriendList.hpp"

class User {
private:
    std::string userName;     // Nom d'utilisateur (utilisé lors de l'inscription ou de la connexion)
    std::string pseudonym;    // Pseudonyme utilisé dans l'application (visible par les autres utilisateurs)
    std::string password;
    int highScore;
    bool isOnline;
    FriendList friendList;

public:
    User(const std::string& userName, const std::string& pseudonym, const std::string& password);

    std::string getUserName() const;
    std::string getPseudonym() const;  // Méthode pour récupérer le pseudonyme
    void setPseudonym(const std::string& newPseudonym);  // Méthode pour changer le pseudonyme

    bool login(const std::string& password);
    void logout();
    bool getIsOnline() const;
    void setHighScore(int score);
    int getHighScore() const;

    // FriendList operations
    void addFriend(const std::string& friendName);
    void removeFriend(const std::string& friendName);
    bool isFriend(const std::string& friendName) const;
    const std::vector<std::string>& getFriends() const;
    size_t getNumberOfFriends() const;
};

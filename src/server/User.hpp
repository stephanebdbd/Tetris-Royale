#pragma once
#include "../include.hpp"
#include "FriendList.hpp"


class User {
private:
    std::string userName;     // Nom d'utilisateur
    std::string pseudonym;    // Pseudonyme
    std::string password;
    int highScore;
    bool isConnected;
    FriendList *friends;    // Liste des amis

public:
    User(const std::string& userName, const std::string& pseudonym, const std::string& password);

    //les informations de l'utilisateur
    std::string getUserName() const;
    std::string getPseudonym() const;
    void setPseudonym(const std::string& newPseudonym);
    //connexion et deconnexion de l'utilisateur
    bool login(const std::string& password);
    void logout();
    void connect();
    void disconnect();
    bool isConnected() const;

    //score de l'utilisateur
    void setHighScore(int score);
    int getHighScore() const;

    //gestion des amis
    void addFriend(std::shared_ptr<User> friendUser);
    void removeFriend(std::shared_ptr<User> friendUser);
    //gerer les demandes d'amis
    void sendFriendRequest(std::shared_ptr<User> friendUser);
    void receiveFriendRequest(std::shared_ptr<User> friendUser);
    void acceptFriendRequest(std::shared_ptr<User> friendUser);
    void rejectFriendRequest(std::shared_ptr<User> friendUser);
    void removeFriendRequest(std::shared_ptr<User> friendUser);
    bool hasfriendRequest(std::shared_ptr<User> friendUser);
    //envoyer un message à un ami
    void sendMessageToFriend(std::string friendName, std::string message);
    //verifier si un utilisateur est un ami
    bool isFriend(std::shared_ptr<User> friendUser);
    //recuperer la liste des amis
    const std::vector<std::shared_ptr<User>>& getFriends() const;
    //comparer deux utilisateurs
    bool operator==(const User& user) const;

    
};


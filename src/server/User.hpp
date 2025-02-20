#pragma once
#include "FriendList.hpp"
#include <memory>
#include <vector>
#include <string>

class User {
private:
    std::string userName;     // Nom d'utilisateur
    std::string pseudonym;    // Pseudonyme
    std::string password;
    int highScore;            // Meilleur score
    bool is_connected;        // Connecté ou non
    FriendList *friends;     // Liste des amis
    int userSocketFd;

public:
    User(const std::string& userName, const std::string& pseudonym, const std::string& password);

    // Informations de l'utilisateur
    std::string getUserName() const;
    std::string getPseudonym() const;
    void setPseudonym(const std::string& newPseudonym);
    int getUserSocketFd() const;
    void setUserSocketFd(int socketFd);

    // Connexion et déconnexion
    bool login(const std::string& password);
    void logout();
    void connect();
    void disconnect();
    bool isConnected() const;

    // Score de l'utilisateur
    void setHighScore(int score);
    int getHighScore() const;

    // Gestion des amis
    void addFriend(std::shared_ptr<User> friendUser);
    void removeFriend(std::shared_ptr<User> friendUser);

    // Gestion des demandes d'amis
    void sendFriendRequest(std::shared_ptr<User> friendUser);
    void receiveFriendRequest(std::shared_ptr<User> friendUser);
    void acceptFriendRequest(std::shared_ptr<User> friendUser);
    void rejectFriendRequest(std::shared_ptr<User> friendUser);
    void removeFriendRequest(std::shared_ptr<User> friendUser);
    bool hasFriendRequest(std::shared_ptr<User> friendUser);

    // Vérifier si un utilisateur est un ami
    bool isFriend(std::shared_ptr<User> friendUser);

    // Récupérer la liste des amis
    const std::vector<std::shared_ptr<User>>& getFriends() const;

    // Comparaison des utilisateurs
    bool operator==(const User& user) const;

};


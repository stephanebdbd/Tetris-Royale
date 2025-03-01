#ifndef FRIENDLIST_HPP
#define FRIENDLIST_HPP

#include "../include.hpp"
#include "User.hpp"

class FriendList {
private:
    // Liste des amis enregistrés (meme on est hors ligne)
    std::vector<std::shared_ptr<User>> friends;
    
    // Liste des demandes d'amis en attente
    std::vector<std::shared_ptr<User>> friendRequests;
    
    // Comptage des amis et demandes d'amis
    int friendRequestCount = 0;
    int friendCount = 0;

public:
    // Constructeur
    FriendList() = default;

    // Getters
    const std::vector<std::shared_ptr<User>>& getFriends() const;
    const std::vector<std::shared_ptr<User>>& getFriendRequests() const;
    
    int getFriendRequestCount() const;
    int getFriendCount() const;

    // Gérer la liste des amis enregistrés
    void addFriend(std::shared_ptr<User> friendUser);
    void removeFriend(std::shared_ptr<User> friendUser);

    // Gérer les demandes d'amis
    void operator>>(std::shared_ptr<User> friendUser);  // Ajouter une demande d'ami
    void operator<<(std::shared_ptr<User> friendUser);  // Accepter une demande d'ami

    // Gérer la liste des amis connectés (en ligne)
    void addOnlineFriend(std::shared_ptr<Client> friendClient);  // Ajouter un ami connecté
    void removeOnlineFriend(std::shared_ptr<Client> friendClient);  // Retirer un ami connecté

    void processClientFriendList(int clientSocket, int clientId);   
    // Destructeur
    ~FriendList() = default;
};

#endif // FRIENDLIST_HPP

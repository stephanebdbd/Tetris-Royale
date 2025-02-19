#include "FriendList.hpp"
#include <algorithm>
#include <iostream>

// Constructeur par défaut
FriendList::FriendList() = default;

// Getters
const std::vector<std::shared_ptr<User>>& FriendList::getFriends() const {
    return friends;
}

const std::vector<std::shared_ptr<User>>& FriendList::getFriendRequests() const {
    return friendRequests;
}

const std::vector<std::shared_ptr<Client>>& FriendList::getOnlineFriends() const {
    return onlineFriends;
}

int FriendList::getFriendRequestCount() const {
    return friendRequestCount;
}

int FriendList::getFriendCount() const {
    return friendCount;
}

// Gérer les amis enregistrés
void FriendList::addFriend(std::shared_ptr<User> friendUser) {
    // Ajouter un ami uniquement s'il n'est pas déjà dans la liste
    auto it = std::find(friends.begin(), friends.end(), friendUser);
    if (it == friends.end()) {
        friends.push_back(friendUser);
        friendCount++;
    }
}

void FriendList::removeFriend(std::shared_ptr<User> friendUser) {
    // Supprimer l'ami de la liste des amis
    auto it = std::find(friends.begin(), friends.end(), friendUser);
    if (it != friends.end()) {
        friends.erase(it);
        friendCount--;
    }
}

// Gérer les demandes d'amis
void FriendList::operator>>(std::shared_ptr<User> friendUser) {
    // Ajouter une demande d'ami uniquement si elle n'existe pas déjà
    auto it = std::find(friendRequests.begin(), friendRequests.end(), friendUser);
    if (it == friendRequests.end()) {
        friendRequests.push_back(friendUser);
        friendRequestCount++;
    }
}

void FriendList::operator<<(std::shared_ptr<User> friendUser) {
    // Accepter une demande d'ami et l'ajouter à la liste des amis
    auto it = std::find(friendRequests.begin(), friendRequests.end(), friendUser);
    if (it != friendRequests.end()) {
        // Retirer la demande d'ami
        friendRequests.erase(it);
        friendRequestCount--;

        // Ajouter l'ami à la liste des amis enregistrés
        addFriend(friendUser);
    }
}

// Gérer la liste des amis connectés (en ligne)
void FriendList::addOnlineFriend(std::shared_ptr<Client> friendClient) {
    // Ajouter un ami en ligne uniquement s'il n'est pas déjà dans la liste
    auto it = std::find_if(onlineFriends.begin(), onlineFriends.end(),
        [&friendClient](const std::shared_ptr<Client>& client) {
            return client == friendClient;
        });

    if (it == onlineFriends.end()) {
        onlineFriends.push_back(friendClient);
    }
}

void FriendList::removeOnlineFriend(std::shared_ptr<Client> friendClient) {
    // Retirer un ami de la liste des amis en ligne
    auto it = std::find(onlineFriends.begin(), onlineFriends.end(), friendClient);
    if (it != onlineFriends.end()) {
        onlineFriends.erase(it);
    }
}

// Destructeur (pas nécessaire ici, car std::shared_ptr gère la mémoire automatiquement)
FriendList::~FriendList() = default;

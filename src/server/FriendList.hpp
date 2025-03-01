#ifndef FRIENDLIST_HPP
#define FRIENDLIST_HPP

#include <string>
#include <vector>
#include <unordered_map>

class FriendList {
private:
    std::unordered_map<std::string, std::vector<std::string>> friends;
    std::unordered_map<std::string, std::vector<std::string>> pendingRequests;
    std::string friendsFile ;  // Fichier pour stocker la liste des amis
    std::string requestsFile; // Fichier pour stocker les demandes d'amis

    // charggement des donnes depuis les fichier 
    std::unordered_map<std::string, std::vector<std::string>> loadFriends();
    void saveFriends(const std::unordered_map<std::string, std::vector<std::string>>& data);
    std::unordered_map<std::string, std::vector<std::string>> loadRequests();
    void saveRequests(const std::unordered_map<std::string, std::vector<std::string>>& data);

public:
    // Constructeur
    FriendList();

    // Vérifier si un utilisateur existe
    bool userExists(const std::string& userId);

    // Vérifier si deux utilisateurs sont amis
    bool areFriends(const std::string& user1, const std::string& user2);

    // Récupérer la liste des amis d'un utilisateur
    std::vector<std::string> getFriendsList(const std::string& user);

    // Récupérer les demandes d'amis en attente
    std::vector<std::string> getPendingRequests(const std::string& user);

    // Ajouter un ami après acceptation
    void addFriend(const std::string& user1, const std::string& user2);

    // Envoyer une demande d'ami
    void sendFriendRequest(const std::string& sender, const std::string& receiver);

    // Supprimer un ami
    void removeFriend(const std::string& user1, const std::string& user2);

    // Supprimer une demande d'ami
    void removeFriendRequest(const std::string& sender, const std::string& receiver);

    // Enregistrer un nouvel utilisateur
    void registerUser(const std::string& username);

    bool acceptFriendRequest(const std::string& user1, const std::string& user2);
};

#endif // FRIENDLIST_HPP

#ifndef UserManager_HPP
#define UserManager_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include "../common/json.hpp"

class UserManager {
private:
    std::string dataFile;  // Fichier JSON pour stocker les données
    nlohmann::json usersData;  // Structure JSON contenant les utilisateurs et amis
    std::string gameInvitationsFile;  // Fichier JSON pour stocker les invitations de jeu
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> gameInvitations;  // Invitations de jeu

    void loadFromFile();  // Charge les données depuis le fichier
    void saveToFile();   // Sauvegarde les données dans le fichier
    void loadGameInvitations();  // Charge les invitations de jeu depuis le fichier
    void saveGameInvitations();  // Sauvegarde les invitations de jeu dans le fichier

public:
    // Constructeur avec un nom de fichier par défaut
    UserManager(const std::string& filename = "users.json", const std::string& filenameInvitation = "gameInvitations.json");

    // Gestion des utilisateurs
    bool registerUser(const std::string& username, const std::string& password);  // Inscrire un utilisateur
    bool loginUser(const std::string& username, const std::string& password);      // Connexion d'un utilisateur
    void updateHighscore(const std::string& username, int newScore);  // Mettre à jour le score élevé
    int getHighScore(const std::string& username) const;             // Récupérer le score élevé

    // Gestion des demandes d'amis
    void sendFriendRequest(const std::string& sender, const std::string& receiver);  // Envoyer une demande d'ami
    void acceptFriendRequest(const std::string& user, const std::string& friendToAccept);  // Accepter une demande d'ami
    void rejectFriendRequest(const std::string& user, const std::string& friendToReject);  // Rejeter une demande d'ami
    bool isPendingRequest(const std::string& sender, const std::string& receiver) const;  // Vérifier si une demande est en attente
    void removeFriendRequest(const std::string& sender, const std::string& receiver);  // Supprimer une demande d'ami

    // Gestion des amis
    bool areFriends(const std::string& user1, const std::string& user2) const;  // Vérifier si deux utilisateurs sont amis
    void removeFriend(const std::string& user, const std::string& friendToRemove);  // Supprimer un ami
    std::vector<std::string> getFriendList(const std::string& user) const;  // Récupérer la liste des amis
    std::vector<std::string> getRequestList(const std::string& user) const;  // Récupérer la liste des demandes d'amis
    bool userExists(const std::string& userId) const;  // Vérifier si un utilisateur existe
    bool userNotExists(const std::string& username) const;  // Vérifier si un utilisateur n'existe pas

    // Gestion des scores
    std::vector<std::pair<std::string, int>> getRanking() const;  // Récupérer le classement des utilisateurs

    // Authentification
    bool authenticateUser(const std::string& username, const std::string& password) const;  // Authentifier un utilisateur

    // Gestion des invitations de jeu
    void sendInvitationToFriend(const std::string& sender, const std::string& receiver, const std::string& status, const int room);  // Envoyer une invitation de jeu à un ami
    std::vector<std::vector<std::string>> getListGameRequest(const std::string& user);  // Récupérer la liste des demandes de jeu
};

#endif // UserManager_HPP
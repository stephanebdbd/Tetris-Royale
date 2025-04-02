#include "UserManager.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

UserManager::UserManager(const std::string& filename,const  std::string& filenameInvitation) : dataFile(filename), gameInvitationsFile(filenameInvitation) {
    loadFromFile();
    loadGameInvitations();
}

// Charge les données depuis le fichier JSON
void UserManager::loadFromFile() {
    std::ifstream file(dataFile);
    if (file.is_open()) {
        try {
            file >> usersData;
        } catch (...) {
            usersData = nlohmann::json::object();
        }
    }
}

// Sauvegarde les données dans le fichier JSON
void UserManager::saveToFile() {
    std::ofstream file(dataFile);
    if (file.is_open()) {
        file << usersData.dump(4);
    }
}

// Inscription d'un utilisateur avec mot de passe
bool UserManager::registerUser(const std::string& username, const std::string& password) {
    if (!usersData.contains(username)) {
        usersData[username] = {
            {"password", password},
            {"friends", nlohmann::json::array()},
            {"requests", nlohmann::json::array()},
            {"highScore", 0}
        };
        saveToFile();
        return true;
    }
    return false;
}

// Connexion d'un utilisateur
bool UserManager::loginUser(const std::string& username, const std::string& password) {
    if (usersData.contains(username)) {
        return usersData[username]["password"] == password;
    }
    return false;
}

// Met à jour le score élevé de l'utilisateur
void UserManager::updateHighscore(const std::string& username, int newScore) {
    if (usersData.contains(username)) {
        int currentHighScore = usersData[username]["highScore"];
        if (newScore > currentHighScore) {
            usersData[username]["highScore"] = newScore;
            saveToFile();
        }
    }
}

// Récupère le score élevé de l'utilisateur
int UserManager::getHighScore(const std::string& username) const {
    if (usersData.contains(username)) {
        return usersData[username]["highScore"];
    }
    return 0;
}

// Envoie une demande d'ami
void UserManager::sendFriendRequest(const std::string& sender, const std::string& receiver) {
    if (!userExists(sender) || !userExists(receiver) || sender == receiver) return;
    if (isPendingRequest(sender, receiver) || areFriends(sender, receiver)) return;

    usersData[receiver]["requests"].push_back(sender);
    saveToFile();
}

// Accepte une demande d'ami
void UserManager::acceptFriendRequest(const std::string& user, const std::string& friendToAccept) {
    if (!userExists(user) || !userExists(friendToAccept)) return;

    auto& requests = usersData[user]["requests"];
    auto it = std::find(requests.begin(), requests.end(), friendToAccept);
    if (it != requests.end()) {
        requests.erase(it);
        usersData[user]["friends"].push_back(friendToAccept);
        usersData[friendToAccept]["friends"].push_back(user);
        saveToFile();
    }
}

// Rejette une demande d'ami
void UserManager::rejectFriendRequest(const std::string& user, const std::string& friendToReject) {
    if (!userExists(user)) return;

    auto& requests = usersData[user]["requests"];
    requests.erase(std::remove(requests.begin(), requests.end(), friendToReject), requests.end());
    saveToFile();
}

// Vérifie si une demande d'ami est en attente
bool UserManager::isPendingRequest(const std::string& sender, const std::string& receiver) const {
    const auto& requests = usersData.at(receiver).at("requests");
    return std::find(requests.begin(), requests.end(), sender) != requests.end();
}

// Supprime une demande d'ami
void UserManager::removeFriendRequest(const std::string& sender, const std::string& receiver) {
    if (!userExists(receiver)) return;

    auto& requests = usersData[receiver]["requests"];
    requests.erase(std::remove(requests.begin(), requests.end(), sender), requests.end());
    saveToFile();
}

// Vérifie si deux utilisateurs sont amis
bool UserManager::areFriends(const std::string& user1, const std::string& user2) const {
    if (!userExists(user1) || !userExists(user2)) return false;

    const auto& friends = usersData.at(user1).at("friends");
    return std::find(friends.begin(), friends.end(), user2) != friends.end();
}

// Supprime un ami
void UserManager::removeFriend(const std::string& user, const std::string& friendToRemove) {
    if (!areFriends(user, friendToRemove)) return;

    auto& userFriends = usersData[user]["friends"];
    auto& friendFriends = usersData[friendToRemove]["friends"];

    userFriends.erase(std::remove(userFriends.begin(), userFriends.end(), friendToRemove), userFriends.end());
    friendFriends.erase(std::remove(friendFriends.begin(), friendFriends.end(), user), friendFriends.end());

    saveToFile();
}

// Récupère la liste des amis
std::vector<std::string> UserManager::getFriendList(const std::string& user) const {
    std::vector<std::string> friendsList;
    if (userExists(user)) {
        for (const auto& friendName : usersData.at(user).at("friends")) {
            friendsList.push_back(friendName);
        }
    }
    return friendsList;
}

// Récupère la liste des demandes d'amis
std::vector<std::string> UserManager::getRequestList(const std::string& user) const {
    std::vector<std::string> requestList;
    if (userExists(user)) {
        for (const auto& request : usersData.at(user).at("requests")) {
            requestList.push_back(request);
        }
    }
    return requestList;
}

// Vérifie si un utilisateur existe
bool UserManager::userExists(const std::string& userId) const {
    return usersData.contains(userId);
}

// Vérifie si un utilisateur n'existe pas
bool UserManager::userNotExists(const std::string& username) const {
    return !usersData.contains(username);
}

// Récupère le classement des utilisateurs
std::vector<std::pair<std::string, int>> UserManager::getRanking() const {
    std::vector<std::pair<std::string, int>> ranking;
    // Parcours des utilisateurs et récupération des noms et scores
    for (const auto& [username, data] : usersData.items()) {
        int score = data["highScore"];
        ranking.emplace_back(username, score);
    }

    // Tri décroissant par rapport au highscore
    std::sort(ranking.begin(), ranking.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;  // Trie par le score élevé
    });

    return ranking;
}

// Méthode pour authentifier un utilisateur (vérifier le nom d'utilisateur et le mot de passe)
bool UserManager::authenticateUser(const std::string& username, const std::string& password) const {
    // Vérifie si l'utilisateur existe dans les données
    if (usersData.contains(username)) {
        // Compare le mot de passe fourni avec celui stocké dans les données
        return usersData[username]["password"] == password;
    }
    return false;  // Si l'utilisateur n'existe pas, retour false
}

// Envoie une invitation de jeu à un ami
void UserManager::sendInvitationToFriend(const std::string& sender, const std::string& receiver, const std::string& status, const int room) {
    std::string room_number = std::to_string(room);

    if (sender == receiver) {
        std::cout << "Vous ne pouvez pas vous envoyer une demande de jeu à vous-même." << std::endl;
        return;
    }

    auto& receiverInvitations = gameInvitations[receiver];

    for (const auto& invitation : receiverInvitations) {
        if (invitation[0] == sender && invitation[2] == room_number) {  // Vérification plus stricte
            std::cout << "Une invitation pour cette salle a déjà été envoyée à " << receiver << "." << std::endl;
            return;
        }
    }

    // Ajouter l'invitation
    receiverInvitations.push_back({sender, status, room_number});
    
    // Sauvegarde
    saveGameInvitations();
}

// Récupère la liste des demandes de jeu
std::vector<std::vector<std::string>> UserManager::getListGameRequest(const std::string& user) {
    return gameInvitations[user];
}

// Sauvegarde les invitations de jeu dans le fichier JSON
void UserManager::saveGameInvitations() {
    std::ofstream file(gameInvitationsFile);
    if (file.is_open()) {
        file << nlohmann::json(gameInvitations).dump(4);
    }
}

// Charge les invitations de jeu depuis le fichier JSON
void UserManager::loadGameInvitations() {
    std::ifstream file(gameInvitationsFile);
    if (file.is_open()) {
        try {
            nlohmann::json jsonData;
            file >> jsonData;
            gameInvitations = jsonData.get<std::unordered_map<std::string, std::vector<std::vector<std::string>>>>();
        } catch (...) {
            gameInvitations = {};
        }
    }
}
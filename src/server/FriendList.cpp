#include "FriendList.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Constructeur : initialise les noms de fichiers
FriendList::FriendList() : friendsFile("friendsList.txt"), requestsFile("friendsRequest.txt") {
    friends = loadFriends();
    pendingRequests = loadRequests();
    }

// Charger la liste des amis depuis le fichier
std::unordered_map<std::string, std::vector<std::string>> FriendList::loadFriends() {
    //std::unordered_map<std::string, std::vector<std::string>> friends;
    std::ifstream file(friendsFile);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << friendsFile << " pour lecture." << std::endl;
        return friends; // Retourner une liste vide si le fichier ne peut pas être ouvert
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string user = line.substr(0, colonPos);
            std::string friendsListStr = line.substr(colonPos + 1);

            // Transformer la chaîne des amis en un vecteur
            std::vector<std::string> userFriends;
            size_t pos = 0;
            while ((pos = friendsListStr.find(',')) != std::string::npos) {
                userFriends.push_back(friendsListStr.substr(0, pos));
                friendsListStr.erase(0, pos + 1);
            }
            if (!friendsListStr.empty()) {
                userFriends.push_back(friendsListStr); // Dernier ami
            }

            friends[user] = userFriends;
        }
    }

    file.close();
    return friends;
}






// Charger les demandes d'amis
std::unordered_map<std::string, std::vector<std::string>> FriendList::loadRequests() {
    std::unordered_map<std::string, std::vector<std::string>> loadedRequests;
    std::ifstream file(requestsFile);

    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << requestsFile << " pour lecture." << std::endl;
        return loadedRequests;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t colonPos = line.find(":");
        if (colonPos != std::string::npos) {
            std::string user = line.substr(0, colonPos);
            std::string requestsList = line.substr(colonPos + 1);
            std::vector<std::string> userRequests;
            size_t pos = 0;
            while ((pos = requestsList.find(',')) != std::string::npos) {
                userRequests.push_back(requestsList.substr(0, pos));
                requestsList.erase(0, pos + 1);
            }
            if (!requestsList.empty()) {
                userRequests.push_back(requestsList);
            }

            loadedRequests[user] = userRequests;
        }
    }

    file.close();
    return loadedRequests;
}

// Sauvegarder les demandes d'amis
void FriendList::saveRequests(const std::unordered_map<std::string, std::vector<std::string>>& data) {
    std::ofstream file(requestsFile);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << requestsFile << " pour écriture." << std::endl;
        return;
    }

    for (const auto& [sender, receivers] : data) {
        file << sender << ":";
        for (size_t i = 0; i < receivers.size(); ++i) {
            file << receivers[i];
            if (i < receivers.size() - 1) file << ",";
        }
        file << "\n";
    }
    file.close();
}

// Vérifier si un utilisateur existe
bool FriendList::userExists(const std::string& userId) {
    auto friends = loadFriends();
    return friends.find(userId) != friends.end();
}

// Vérifier si deux utilisateurs sont déjà amis
bool FriendList::areFriends(const std::string& user1, const std::string& user2) {
    auto friends = loadFriends();
    auto it = friends.find(user1);
    if (it != friends.end()) {
        return std::find(it->second.begin(), it->second.end(), user2) != it->second.end();
    }
    return false;
}

// Ajouter une demande d'ami
void FriendList::sendFriendRequest(const std::string& sender, const std::string& receiver) {
    auto requests = loadRequests();
    if (std::find(requests[sender].begin(), requests[sender].end(), receiver) == requests[sender].end()) {
        requests[sender].push_back(receiver);
        saveRequests(requests);
    }
}

// Accepter une demande d'ami et l'ajouter à la liste
bool FriendList::acceptFriendRequest(const std::string& user1, const std::string& user2) {
    auto requests = loadRequests();
    auto it = std::find(requests[user2].begin(), requests[user2].end(), user1);
    
    if (it != requests[user2].end()) {
        requests[user2].erase(it);
        saveRequests(requests);

        auto friends = loadFriends();
        friends[user1].push_back(user2);
        friends[user2].push_back(user1);
        saveFriends(friends);
        return true;
    }
    return false;
}

// Supprimer un ami
void FriendList::removeFriend(const std::string& user1, const std::string& user2) {
    auto friends = loadFriends();

    auto it1 = std::find(friends[user1].begin(), friends[user1].end(), user2);
    if (it1 != friends[user1].end()) {
        friends[user1].erase(it1);
    }

    auto it2 = std::find(friends[user2].begin(), friends[user2].end(), user1);
    if (it2 != friends[user2].end()) {
        friends[user2].erase(it2);
    }

    saveFriends(friends);
}

// Supprimer une demande d'ami
void FriendList::removeFriendRequest(const std::string& sender, const std::string& receiver) {
    auto requests = loadRequests();
    auto it = std::find(requests[sender].begin(), requests[sender].end(), receiver);
    
    if (it != requests[sender].end()) {
        requests[sender].erase(it);
        saveRequests(requests);
    }
}

// Récupérer la liste des amis d'un utilisateur
std::vector<std::string> FriendList::getFriendsList(const std::string& user) {
    auto friends = loadFriends();
    return friends[user]; // Retourne une liste vide si l'utilisateur n'existe pas
}

// Récupérer les demandes d'amis en attente
std::vector<std::string> FriendList::getPendingRequests(const std::string& user) {
    auto requests = loadRequests();
    return requests[user]; // Retourne une liste vide si pas de demande
}



void FriendList::saveFriends(const std::unordered_map<std::string, std::vector<std::string>>& data) {
    // Ouvrir le fichier pour ajouter sans écraser les données existantes
    std::ofstream file(friendsFile, std::ios::out); // std::ios::out est suffisant, pas besoin de std::ios::trunc

    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << friendsFile << " pour écriture." << std::endl;
        return;
    }

    for (const auto& [user, friends] : data) {
        file << user << ":";
        for (size_t i = 0; i < friends.size(); ++i) {
            file << friends[i];
            if (i < friends.size() - 1) file << ",";
        }
        file << "\n";
    }

    file.close();
}
void FriendList::registerUser(const std::string& username) {
    // Charger les amis existants
    std::unordered_map<std::string, std::vector<std::string>> friends = loadFriends();

    // Vérifier si l'utilisateur existe déjà
    if (friends.find(username) != friends.end()) {
        std::cout << "Utilisateur " << username << " existe déjà. Aucun ajout effectué." << std::endl;
        return; 
    }

    // Ajouter l'utilisateur avec une liste d'amis vide
    friends[username] = {}; 

    // Sauvegarder les mises à jour (n'écrase pas les données existantes)
    saveFriends(friends); 

    std::cout << "Utilisateur " << username << " ajouté avec succès." << std::endl;
}

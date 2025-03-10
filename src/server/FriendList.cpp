#include "FriendList.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Constructeur : initialise les noms de fichiers
FriendList::FriendList() : friendsFile("friendsList.txt"), requestsFile("friendsRequest.txt"), gameInvitationsFile("gameInvitations.txt") {
    friends = loadFriends();
    requests = loadRequests();
    gameInvitations = loadGameInvitations();
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
void FriendList::saveRequests() {
    std::ofstream file(requestsFile);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << requestsFile << " pour écriture." << std::endl;
        return;
    }

    for (const auto& [sender, receivers] : requests) {
        file << sender << ":";
        for (size_t i = 0; i < receivers.size(); ++i) {
            file << receivers[i];
            if (i < receivers.size() - 1) file << ",";
        }
        file << "\n";
    }
    file.close();
}

bool FriendList::isPendingRequest(const std::string& sender, const std::string& receiver) {
    // Vérifie si 'receiver' a une demande en attente de 'sender'
    auto it = requests.find(receiver);
    if (it != requests.end()) {
        return std::find(it->second.begin(), it->second.end(), sender) != it->second.end();

    }
    return false;
}



void FriendList::saveFriends() {
    // Ouvrir le fichier pour ajouter sans écraser les données existantes
    std::ofstream file(friendsFile, std::ios::out); // std::ios::out est suffisant, pas besoin de std::ios::trunc

    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << friendsFile << " pour écriture." << std::endl;
        return;
    }

    for (const auto& [user, friends] : friends) {
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

    // Vérifier si l'utilisateur existe déjà
    if (friends.find(username) != friends.end()) {
        std::cout << "Utilisateur " << username << " existe déjà. Aucun ajout effectué." << std::endl;
        return; 
    }

    // Ajouter l'utilisateur avec une liste d'amis vide
    friends[username] = {}; 
    requests[username] = {}; // Initialiser la liste des demandes vides

    // Sauvegarder les mises à jour (n'écrase pas les données existantes)
    saveFriends(); 
    saveRequests();

    std::cout << "Utilisateur " << username << " ajouté avec succès." << std::endl;
}

void FriendList::sendFriendRequest(const std::string& sender, const std::string& receiver) {
 
    // Vérifier que l'utilisateur n'envoie pas une demande à lui-même
    if (sender == receiver) {
        std::cout << "Vous ne pouvez pas envoyer une demande d'ami à vous-même." << std::endl;
        return;
    }

    // Ajouter la demande dans la liste des demandes de l'utilisateur récepteur
    if (requests.find(receiver) == requests.end()) {
        // Si l'utilisateur n'a pas encore de demandes, initialiser la liste
        requests[receiver] = {};
    }
    // Vérifier si la demande existe déjà
    auto& receiverRequests = requests[receiver];
    if (std::find(receiverRequests.begin(), receiverRequests.end(), sender) != receiverRequests.end()) {
        std::cout << "La demande d'ami a déjà été envoyée à " << receiver << "." << std::endl;
        return;
        
    }

    // Ajouter l'expéditeur à la liste des demandes en attente du récepteur
    receiverRequests.push_back(sender);

    // Sauvegarder les demandes après modification
    saveRequests();
    
}

void FriendList::acceptFriendRequest(const std::string& user, const std::string& friendToAccept) {
    // Vérifier si la demande existe

    auto& userRequests = requests[user];
    auto it = std::find(userRequests.begin(), userRequests.end(), friendToAccept);
    if (it == userRequests.end()) {
        std::cout << "Aucune demande d'ami de " << friendToAccept << " trouvée." << std::endl;
        return;
    }

    friends[user].push_back(friendToAccept);
    friends[friendToAccept].push_back(user);

    // Supprimer la demande de la liste
    userRequests.erase(it);
    // Sauvegarder les mises à jour des amis et des demandes
    saveFriends();
    saveRequests();
}
// Méthode pour rejeter une demande d'ami
void FriendList::rejectFriendRequest(const std::string& user, const std::string& friendToReject) {
    // Vérifier si la demande existe
    auto& userRequests = requests[user];
    auto it = std::find(userRequests.begin(), userRequests.end(), friendToReject);
    if (it == userRequests.end()) {
        std::cout << "Aucune demande d'ami de " << friendToReject << " trouvée." << std::endl;
        return;
    }

    // Retirer la demande d'ami sans ajouter l'utilisateur à la liste des amis
    userRequests.erase(it);

    // Sauvegarder les mises à jour des demandes
    saveRequests();

}

bool FriendList::userExists(const std::string& username) const {
    return friends.find(username) != friends.end();
}

// Vérifie si deux utilisateurs sont amis
bool FriendList::areFriends(const std::string& user1, const std::string& user2) const {
    if (!userExists(user1) || !userExists(user2)) {
        return false; // Un des utilisateurs n'existe pas
    }
    const auto& userFriends = friends.at(user1);
    return std::find(userFriends.begin(), userFriends.end(), user2) != userFriends.end();
}
void FriendList::removeFriend(const std::string& user, const std::string& friendToRemove) {
    if (!areFriends(user, friendToRemove)) {
        std::cout << user << " et " << friendToRemove << " ne sont pas amis." << std::endl;
        return;
    }

    // Supprimer de la liste de l'utilisateur
    auto& userFriends = friends[user];
    userFriends.erase(std::remove(userFriends.begin(), userFriends.end(), friendToRemove), userFriends.end());

    // Supprimer de la liste de l'ami
    auto& friendFriends = friends[friendToRemove];
    friendFriends.erase(std::remove(friendFriends.begin(), friendFriends.end(), user), friendFriends.end());

    // Sauvegarde des modifications
    saveFriends();
    std::cout << friendToRemove << " a été retiré de la liste d'amis de " << user << "." << std::endl;
}

// Supprime une demande d'ami

void FriendList::removeFriendRequest(const std::string& sender, const std::string& receiver) {
    if (requests.find(receiver) == requests.end()) {
        std::cout << "Aucune demande d'ami trouvée de " << sender << " à " << receiver << "." << std::endl;
        return;
    }

    auto& req = requests[receiver];
    auto it = std::remove(req.begin(), req.end(), sender);
    
    if (it != req.end()) {
        req.erase(it, req.end());
        saveRequests();
        std::cout << "Demande d'ami de " << sender << " à " << receiver << " supprimée." << std::endl;
    } else {
        std::cout << "Aucune demande d'ami trouvée de " << sender << " à " << receiver << "." << std::endl;
    }
}
std::vector<std::string> FriendList::getFriendList(const std::string& user) const {
    auto it = friends.find(user);
    if (it != friends.end()) {
        return it->second;
    }
    return {}; // Retourne une liste vide si l'utilisateur n'a pas d'amis
}

// Récupère la liste des demandes d'amis en attente pour un utilisateur
std::vector<std::string> FriendList::getRequestList(const std::string& user) const {
    auto it = requests.find(user);
    if (it != requests.end()) {
        return it->second;
    }
    return {}; // Retourne une liste vide si l'utilisateur n'a pas de demandes en attente
}



//******************************************************************* */
// Ce qui suit est pour les invitations de jeu

// invitation au jeu soit pour jouer ou observer

std::unordered_map<std::string, std::vector<std::vector<std::string>>> FriendList::loadGameInvitations() {
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> invitToGame;
    std::ifstream file(gameInvitationsFile);
    
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << gameInvitationsFile << " pour lecture." << std::endl;
        return invitToGame; // Retourner une liste vide si le fichier ne peut pas être ouvert
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t colonPos = line.find(':');   // Trouver la position du ":" sebder:  receiver.status
        size_t dotPos = line.find('.');    // Trouver la position du "."
        
        if (colonPos != std::string::npos && dotPos != std::string::npos) {
            std::string sender = line.substr(0, colonPos); // Récupérer le sender
            std::string receiverList = line.substr(colonPos + 1);  // Récupérer la liste des invitations

            std::vector<std::vector<std::string>> senderGameInvitations;
            
            // Analyser la liste des invitations (séparées par des virgules)
            size_t pos = 0;
            while ((pos = receiverList.find(',')) != std::string::npos) {
                std::string invitation = receiverList.substr(0, pos);  // Extraire l'invitation avant la virgule
                size_t dotPos = invitation.find(".");  // Trouver le "."     oumaima: ali.observer
                size_t dot1Pos = invitation.find(".", dotPos + 1);  // Trouver le "."
                if (dotPos != std::string::npos) {
                    std::string receiver = invitation.substr(0, dotPos); // Receiver avant le "."
                    std::string status = invitation.substr(dotPos + 1, dot1Pos - dotPos - 1); // Status après le "."
                    std::string room = invitation.substr(dot1Pos + 1); // Receiver après le "."
                    senderGameInvitations.push_back({receiver, status, room});  // Ajouter la paire à la liste
                }
                
                receiverList.erase(0, pos + 1);  // Supprimer l'invitation traitée
            }

            // Ajouter l'ensemble des invitations pour le sender dans le map
            invitToGame[sender] = senderGameInvitations;
        }
    }

    file.close();
    return invitToGame;
}




// sauvegarder les demandes de jeu envoiyé par mes amis 
void FriendList::saveGameInvitations(){
    std::ofstream file(gameInvitationsFile);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << gameInvitationsFile << " pour écriture." << std::endl;
        return;
    }

    for (const auto& [sender, receivers] : gameInvitations) {
        file << sender << ":";
        for (size_t i = 0; i < receivers.size(); ++i) {
            file << receivers[i][0] << "." << receivers[i][1] << "." << receivers[i][2];
            if (i < receivers.size() - 1) file << ",";
        }
        file << "\n";
    }
    file.close();
}



// Méthode pour envoyer une invitation de jeu
void FriendList::sendInvitationToFriend(const std::string& sender, const std::string& receiver, const std::string& status, const int room) {
    std::string room_number = std::to_string(room);

    if (sender == receiver) {
        std::cout << "Vous ne pouvez pas envoyer une demande de jeu à vous-même." << std::endl;
        return;
    }

    // Ajouter la demande dans la liste des demandes de l'utilisateur récepteur
    if (gameInvitations.find(receiver) == gameInvitations.end()) {
        // Si l'utilisateur n'a pas encore de demandes, initialiser la liste
        gameInvitations[receiver] = {};
    }
    // Vérifier si la demande existe déjà
    auto& gameInvitation = gameInvitations[receiver];
    for (const auto& invitation : gameInvitation) {
        if (invitation[0] == sender) {
            std::cout << "La demande de jeu a déjà été envoyée à " << receiver << "." << std::endl;
            return;
        }
    }

    // Ajouter l'expéditeur à la liste des demandes en attente du récepteur
    gameInvitations[receiver].push_back({sender,status,room_number});

    // Sauvegarder les demandes après modification
    saveGameInvitations();

}
std::vector<std::vector<std::string>> FriendList:: getListGameRequest(const std::string& user) {
    return gameInvitations[user];
}


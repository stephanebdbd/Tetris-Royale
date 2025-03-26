/*#include "FriendList.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

// Constructeur par défaut qui délègue au constructeur avec paramètres
FriendList::FriendList() : friendsFile("Friends/friends.json"), requestsFile("Friends/requests.json"), gameInvitationsFile("gameInvitations.txt") {
    // Créer le répertoire "Friends" s'il n'existe pas
    std::filesystem::create_directories("Friends");
    // Charger les listes d'amis et de demandes d'amis
    loadFriends();
    loadRequests();
    gameInvitations = loadGameInvitations();
}

void FriendList::loadFriends() {
    std::ifstream inputFile(friendsFile);
    if (!inputFile) {
        std::cerr << "Fichier " << friendsFile << " introuvable. Création d'un fichier vide." << std::endl;
        saveFriends();
        return;
    }
    try {
        nlohmann::json j;
        inputFile >> j;
        if (j.is_object()) {
            for (const auto& [user, friendList] : j.items()) {
                friends[user] = friendList.get<std::vector<std::string>>();
            }
        }
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Erreur de parsing JSON (friends) : " << e.what() << std::endl;
    }
}

void FriendList::saveFriends() {
    std::ofstream outputFile(friendsFile);
    if (!outputFile) {
        std::cerr << "Erreur d'ouverture du fichier " << friendsFile << " pour l'écriture." << std::endl;
        return;
    }
    nlohmann::json j(friends);
    outputFile << j.dump(4);  // Formatage avec indentation
}

void FriendList::loadRequests() {
    std::ifstream inputFile(requestsFile);
    if (!inputFile) {
        std::cerr << "Fichier " << requestsFile << " introuvable. Création d'un fichier vide." << std::endl;
        saveRequests();
        return;
    }
    try {
        nlohmann::json j;
        inputFile >> j;
        if (j.is_object()) {
            for (const auto& [user, requestList] : j.items()) {
                requests[user] = requestList.get<std::vector<std::string>>();
            }
        }
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Erreur de parsing JSON (requests) : " << e.what() << std::endl;
    }
}

void FriendList::saveRequests() {
    std::ofstream outputFile(requestsFile);
    if (!outputFile) {
        std::cerr << "Erreur d'ouverture du fichier " << requestsFile << " pour l'écriture." << std::endl;
        return;
    }
    nlohmann::json j(requests);
    outputFile << j.dump(4);
}

void FriendList::registerUser(const std::string& username) {
    if (!userExists(username)) {
        friends[username] = {};
        requests[username] = {};
        saveFriends();
        saveRequests();
    }
}

void FriendList::sendFriendRequest(const std::string& sender, const std::string& receiver) {
    if (userExists(receiver) && sender != receiver && !isPendingRequest(sender, receiver) && !areFriends(sender, receiver)) {
        requests[receiver].push_back(sender);
        saveRequests();
    }
}

void FriendList::acceptFriendRequest(const std::string& user, const std::string& friendToAccept) {
    auto& userRequests = requests[user];
    auto it = std::find(userRequests.begin(), userRequests.end(), friendToAccept);
    if (it != userRequests.end()) {
        userRequests.erase(it);
        friends[user].push_back(friendToAccept);
        friends[friendToAccept].push_back(user);
        saveFriends();
        saveRequests();
    }
}

void FriendList::rejectFriendRequest(const std::string& user, const std::string& friendToReject) {
    auto& userRequests = requests[user];
    auto it = std::find(userRequests.begin(), userRequests.end(), friendToReject);
    if (it != userRequests.end()) {
        userRequests.erase(it);
        saveRequests();
    }
}

bool FriendList::areFriends(const std::string& user1, const std::string& user2) const {
    if (!userExists(user1) || !userExists(user2)) return false;
    const auto& user1Friends = friends.at(user1);
    return std::find(user1Friends.begin(), user1Friends.end(), user2) != user1Friends.end();
}

bool FriendList::userExists(const std::string& userId) const {
    return friends.find(userId) != friends.end();
}

void FriendList::removeFriend(const std::string& user, const std::string& friendToRemove) {
    if (areFriends(user, friendToRemove)) {
        auto& userFriends = friends[user];
        userFriends.erase(std::remove(userFriends.begin(), userFriends.end(), friendToRemove), userFriends.end());
        auto& friendFriends = friends[friendToRemove];
        friendFriends.erase(std::remove(friendFriends.begin(), friendFriends.end(), user), friendFriends.end());
        saveFriends();
    }
}

void FriendList::removeFriendRequest(const std::string& sender, const std::string& receiver) {
    auto& receiverRequests = requests[receiver];
    receiverRequests.erase(std::remove(receiverRequests.begin(), receiverRequests.end(), sender), receiverRequests.end());
    saveRequests();
}

std::vector<std::string> FriendList::getFriendList(const std::string& user) const {
    if (userExists(user)) return friends.at(user);
    return {};
}

std::vector<std::string> FriendList::getRequestList(const std::string& user) const {
    if (userExists(user)) return requests.at(user);
    return {};
}

bool FriendList::isPendingRequest(const std::string& sender, const std::string& receiver) const {
    if (!userExists(receiver)) return false;
    const auto& receiverRequests = requests.at(receiver);
    return std::find(receiverRequests.begin(), receiverRequests.end(), sender) != receiverRequests.end();
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
        for (std::size_t i = 0; i < receivers.size(); ++i) {
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


std::unordered_map<std::string, std::vector<std::vector<std::string>>> FriendList::loadGameInvitations() {
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> invitToGame;
    std::ifstream file(gameInvitationsFile);
    
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << gameInvitationsFile << " pour lecture." << std::endl;
        return invitToGame; // Retourner une liste vide si le fichier ne peut pas être ouvert
    }

    std::string line;
    while (std::getline(file, line)) {
        std::size_t colonPos = line.find(':');   // Trouver la position du ":" sebder:  receiver.status
        std::size_t dotPos = line.find('.');    // Trouver la position du "."
        
        if (colonPos != std::string::npos && dotPos != std::string::npos) {
            std::string sender = line.substr(0, colonPos); // Récupérer le sender
            std::string receiverList = line.substr(colonPos + 1);  // Récupérer la liste des invitations

            std::vector<std::vector<std::string>> senderGameInvitations;
            
            // Analyser la liste des invitations (séparées par des virgules)
            std::size_t pos = 0;
            while ((pos = receiverList.find(',')) != std::string::npos) {
                std::string invitation = receiverList.substr(0, pos);  // Extraire l'invitation avant la virgule
                std::size_t dotPos = invitation.find(".");  // Trouver le "."     oumaima: ali.observer
                std::size_t dot1Pos = invitation.find(".", dotPos + 1);  // Trouver le "."
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


*/
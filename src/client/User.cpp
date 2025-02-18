#include "User.hpp"
#include <iostream>

User::User(const std::string& userName, const std::string& pseudonym, const std::string& password)
    : userName(userName), pseudonym(pseudonym), password(password), highScore(0), isConnected(false), friends(nullptr) {}

// Obtenir le nom d'utilisateur
std::string User::getUserName() const {
    return userName;
}

// Obtenir le pseudonyme
std::string User::getPseudonym() const {
    return pseudonym;
}

// Modifier le pseudonyme
void User::setPseudonym(const std::string& newPseudonym) {
    pseudonym = newPseudonym;
}

// Connexion de l'utilisateur
bool User::login(const std::string& inputPassword) {
    if (password == inputPassword) {
        isConnected = true;
        return true;
    }
    return false;
}

// Déconnexion de l'utilisateur
void User::logout() {
    isConnected = false;
}

// Connecter manuellement un utilisateur (ex: administrateur)
void User::connect() {
    isConnected = true;
}

// Déconnecter manuellement un utilisateur
void User::disconnect() {
    isConnected = false;
}

// Vérifier si l'utilisateur est connecté
bool User::isConnected() const {
    return isConnected;
}

// Définir un nouveau high score
void User::setHighScore(int score) {
    if (score > highScore) {
        highScore = score;
    }
}

// Récupérer le high score
int User::getHighScore() const {
    return highScore;
}

// Ajouter un ami
void User::addFriend(std::shared_ptr<User> friendUser) {
    if (!isFriend(friendUser)) {
        friends->addFriend(friendUser);
    }
}

// Supprimer un ami
void User::removeFriend(std::shared_ptr<User> friendUser) {
    friends->removeFriend(friendUser);
}

// Envoyer une demande d'ami
void User::sendFriendRequest(std::shared_ptr<User> friendUser) {
    friendUser->receiveFriendRequest(std::make_shared<User>(*this));
}

// Recevoir une demande d'ami
void User::receiveFriendRequest(std::shared_ptr<User> friendUser) {
    friends->receiveRequest(friendUser);
}

// Accepter une demande d'ami
void User::acceptFriendRequest(std::shared_ptr<User> friendUser) {
    friends->acceptRequest(friendUser);
}

// Refuser une demande d'ami
void User::rejectFriendRequest(std::shared_ptr<User> friendUser) {
    friends->rejectRequest(friendUser);
}

// Supprimer une demande d'ami
void User::removeFriendRequest(std::shared_ptr<User> friendUser) {
    friends->removeRequest(friendUser);
}

// Vérifier si une demande d'ami existe
bool User::hasFriendRequest(std::shared_ptr<User> friendUser) {
    return friends->hasRequest(friendUser);
}

// Vérifier si un utilisateur est un ami
bool User::isFriend(std::shared_ptr<User> friendUser) {
    return friends->isFriend(friendUser);
}

// Récupérer la liste des amis
const std::vector<std::shared_ptr<User>>& User::getFriends() const {
    return friends->getFriendList();
}

// Comparer deux utilisateurs
bool User::operator==(const User& user) const {
    return userName == user.userName;
}

// Envoyer un messageà un ami
void User::sendMessageToFriend(const std::string& friendName, const std::string& message) {
    std::cout << "Message envoyé à " << friendName << ": " << message << std::endl;
}

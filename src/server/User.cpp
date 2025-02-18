#include "User.hpp"
#include <iostream>
#include <algorithm>
#include <memory>

// Constructeur
User::User(const std::string& userName, const std::string& pseudonym, const std::string& password) 
    : userName(userName), pseudonym(pseudonym), password(password), highScore(0), isConnected(false) {
    friends = new FriendList();  // Crée une liste d'amis vide
}

// Getters et Setters pour les informations de l'utilisateur
std::string User::getUserName() const {
    return userName;
}

std::string User::getPseudonym() const {
    return pseudonym;
}

void User::setPseudonym(const std::string& newPseudonym) {
    pseudonym = newPseudonym;
}

// Connexion et déconnexion de l'utilisateur
bool User::login(const std::string& password) {
    if (this->password == password) {
        connect();  // Appelle la méthode connect() si le mot de passe est correct
        return true;
    }
    return false;
}

void User::logout() {
    disconnect();
}

void User::connect() {
    if (!isConnected) {
        isConnected = true;
        std::cout << "Utilisateur " << userName << " connecté." << std::endl;
    }
}

void User::disconnect() {
    if (isConnected) {
        isConnected = false;
        std::cout << "Utilisateur " << userName << " déconnecté." << std::endl;
    }
}

bool User::isConnected() const {
    return isConnected;
}

// Gestion du score de l'utilisateur
void User::setHighScore(int score) {
    highScore = score;
}

int User::getHighScore() const {
    return highScore;
}

// Gestion des amis
void User::addFriend(std::shared_ptr<User> friendUser) {
    if (!isFriend(friendUser)) {
        friends->addFriend(friendUser);
        std::cout << "Ami ajouté: " << friendUser->getUserName() << std::endl;
    } else {
        std::cout << "Cet utilisateur est déjà un ami." << std::endl;
    }
}

void User::removeFriend(std::shared_ptr<User> friendUser) {
    if (isFriend(friendUser)) {
        friends->removeFriend(friendUser);
        std::cout << "Ami retiré: " << friendUser->getUserName() << std::endl;
    } else {
        std::cout << "Cet utilisateur n'est pas un ami." << std::endl;
    }
}

// Gestion des demandes d'amis
void User::sendFriendRequest(std::shared_ptr<User> friendUser) {
    std::cout << "Demande d'ami envoyée à: " << friendUser->getUserName() << std::endl;
    friendUser->receiveFriendRequest(shared_from_this());
}

void User::receiveFriendRequest(std::shared_ptr<User> friendUser) {
    std::cout << "Demande d'ami reçue de: " << friendUser->getUserName() << std::endl;
}

void User::acceptFriendRequest(std::shared_ptr<User> friendUser) {
    addFriend(friendUser);
    std::cout << "Demande d'ami acceptée de: " << friendUser->getUserName() << std::endl;
}

void User::rejectFriendRequest(std::shared_ptr<User> friendUser) {
    std::cout << "Demande d'ami rejetée de: " << friendUser->getUserName() << std::endl;
}

void User::removeFriendRequest(std::shared_ptr<User> friendUser) {
    std::cout << "Demande d'ami supprimée de: " << friendUser->getUserName() << std::endl;
}

// Vérifier si un utilisateur est un ami
bool User::isFriend(std::shared_ptr<User> friendUser) {
    return friends->hasFriend(friendUser);
}

// Récupérer la liste des amis
const std::vector<std::shared_ptr<User>>& User::getFriends() const {
    return friends->getFriends();
}

// Comparaison des utilisateurs
bool User::operator==(const User& user) const {
    return userName == user.userName && pseudonym == user.pseudonym;
}

// Envoi de message à un ami
void User::sendMessageToFriend(std::string friendName, std::string message) {
    std::cout << "Envoi du message '" << message << "' à " << friendName << std::endl;
}


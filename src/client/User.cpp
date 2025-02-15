#include "User.hpp"
#include <iostream>

User::User(const std::string& userName, const std::string& pseudonym, const std::string& password)
    : userName(userName), pseudonym(pseudonym), password(password), highScore(0), isOnline(false) {}

std::string User::getUserName() const {
    return userName;
}

std::string User::getPseudonym() const {
    return pseudonym;
}

void User::setPseudonym(const std::string& newPseudonym) {
    pseudonym = newPseudonym;
    std::cout << "Votre pseudonyme a été changé en : " << pseudonym << std::endl;
}

bool User::login(const std::string& password) {
    if (this->password == password) {
        isOnline = true;
        std::cout << pseudonym << " (" << userName << ") est maintenant connecté." << std::endl;
        return true;
    }
    std::cout << "Mot de passe incorrect." << std::endl;
    return false;
}

void User::logout() {
    isOnline = false;
    std::cout << pseudonym << " est maintenant déconnecté." << std::endl;
}

bool User::getIsOnline() const {
    return isOnline;
}

void User::setHighScore(int score) {
    highScore = score;
}

int User::getHighScore() const {
    return highScore;
}

void User::addFriend(const std::string& friendName) {
    friendList.addFriend(friendName);
}

void User::removeFriend(const std::string& friendName) {
    friendList.removeFriend(friendName);
}

bool User::isFriend(const std::string& friendName) const {
    return friendList.isFriend(friendName);
}

const std::vector<std::string>& User::getFriends() const {
    return friendList.getFriends();
}

size_t User::getNumberOfFriends() const {
    return friendList.getNumberOfFriends();
}

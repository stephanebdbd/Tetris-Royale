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

//gestion des amis
void User::addFriend(std::shared_ptr<User> friendUser) {
    friends += friendUser;  
}

void User::removeFriend(std::shared_ptr<User> friendUser) {
    friends -= friendUser;
}

//gerer les demandes des amis
void User::sendFriendRequest(std::shared_ptr<User> friendUser) {
    friendUser->receiveFriendRequest(std::shared_ptr<User>(this));
}

void User::receiveFriendRequest(std::shared_ptr<User> friendUser) {
    friends >> friendUser;
    //send message to friendUser
    //sendMessageToFriend(friendUser->getPseudonym(), "Vous avez une demande d'ami de " + pseudonym);

void User::acceptFriendRequest(std::shared_ptr<User> friendUser) {
    friends += friendUser;
    removeFriendRequest(friendUser);
}

void User::rejectFriendRequest(std::shared_ptr<User> friendUser) {
    //send message to friendUser
    //sendMessageToFriend(friendUser->getPseudonym(), "Votre demande d'ami a été rejetée.");
    removeFriendRequest(friendUser);
}

void User::removeFriendRequest(std::shared_ptr<User> friendUser) {
    friends -= friendUser;
}
bool User::hasFriendRequest(std::shared_ptr<User> friendUser);

//envoyer un message à un ami
void User::sendMessageToFriend(std::string friendName, std::string message) {
    //friendUser->receiveMessage(message);
}

//verfier si un utilisateur est ami
bool User::isFriend(std::shared_ptr<User> friendUser) {
    for (int i = 0; i < friends->getFriendCount(); i++) {
        if (friends[i] == friendUser) {
            return true;
        }
    }
    return false;
}

//operator pour comparer deux utilisateurs
bool User::operator==(const User& user) const {
    return userName == user.userName && pseudonym == user.pseudonym;
}


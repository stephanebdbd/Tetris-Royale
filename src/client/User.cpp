#include "User.hpp"

User::User(const std::string& userName, const std::string& password, int matricule)
    : userName(userName), password(password), matricule(matricule) {}

void User::addFriend(User* friendUser) {
    friendList.addFriend(friendUser);
}

bool User::login(const std::string& passwordInput) {
    if (passwordInput == password) {
        isOnline = true;
        return true;
    }
    return false;
}

void User::logout() {
    isOnline = false;
}

void User::setHighScore(int newHighScore) {
    if (newHighScore > highScore) {
        highScore = newHighScore;
    }
}

int User::getHighScore() const {
    return highScore;
}

bool User::getIsOnline() const {
    return isOnline;
}

std::string User::getUserName() const {
    return userName;
}

int User::getMatricule() const {
    return matricule;
}

FriendList& User::getFriendList() {
    return friendList;
}

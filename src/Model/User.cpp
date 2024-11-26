#include <User.hpp>

User::User(string userName, string password, int matricule) {
    this->userName = userName;
    this->password = password;
    this->matricule = matricule;
}

void User::addFriend(string friendName) {
    friendList.addFriend(friendName);
}

int User::getHighScore() {
    return highScore;
}

int User::getMatricule() {
    return matricule;
}

void Server::removeUser(User user) {
    int matricule = user.getMatricule();
    delete users[matricule];
    users[matricule] = nullptr;
}
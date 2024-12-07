#include "User.hpp"

Server::Server() {
    users = new std::vector<User*>;
}

void Server::createUser(std::string userName, std::string password) {
    int matricule = users->size();
    User* user = new User(userName, password, matricule);
    users->push_back(user);
}

void Server::removeUser(User user) {
    int matricule = user.getMatricule();
    delete (*users)[matricule];
    (*users)[matricule] = nullptr;
}

Server::~Server() {
    int size = users->size();
    for(int i=0; i<size; i++) {
        delete (*users)[i];
    }
    delete users;
}

User::User(std::string userName, std::string password, int matricule) 
    : userName(userName), password(password), matricule(matricule) {
    friendList = new FriendList();
    }


void User::addFriend(std::string friendName) {
    friendList->addFriend(friendName);
}

bool User::login(std::string password) {
    if(this->password == password) {
        isOnline = true;
        return true;
    }
    return false;
}

void User::logout() {
    isOnline = false;
}

bool User::getIsOnline() {
    return isOnline;
}

int User::getHighScore() {
    return highScore;
}

int User::getMatricule() {
    return matricule;
}

void User::setHighScore(int newHighScore) {
    highScore = newHighScore;
}

FriendList::FriendList() {
    listOfFriends = new std::vector<std::string>();
}

Player::Player(std::string userName, std::string password) 
    : User(userName, password) {}

void FriendList::addFriend(std::string friendName) {
    listOfFriends->push_back(friendName);
    numberOfFriends++;
}

void FriendList::removeFriend(std::string friendName) {
    int i=0;
    bool found = false;
    while(i<numberOfFriends && !found) {
        if(listOfFriends->at(i) == friendName) {
            found = true;
            listOfFriends->erase(listOfFriends->begin()+i);
            numberOfFriends--;
        }
        i++;
    }
}

int FriendList::getamountOfFriends() {
    return numberOfFriends;
}

std::vector<std::string>* FriendList::getFriends() {
    return listOfFriends;
}

FriendList::~FriendList() {
    delete listOfFriends;
}

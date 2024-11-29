#pragma once

#include "../include.hpp"
#include "utils.hpp"

class FriendList {
    vector<std::string> listOfFriends;
    int numberOfFriends=0;
public:
    void addFriend(std::string friendName);
    void removeFriend(std::string friendName);
    int getamountOfFriends();
};

class User {
    Server* server;
    std::string userName;
    std::string password;
    int highScore=0;
    int matricule;
    FriendList friendList;
    bool isOnline=false;
public:
    User(std::string userName, std::string password, int matricule);
    void addFriend(std::string friendName);
    void rename(std::string newName);
    int getHighScore();
    int getMatricule();
};

class Player : public User {};

class Server {
    vector<User*> users;
public:
    void createUser(std::string userName, std::string password);
    void addUser(User user);
    void removeUser(User user);
    void login(std::string userName, std::string password);
    void optimise(); // A voir comment on va faire pour rendre le vecteur plus petit
};
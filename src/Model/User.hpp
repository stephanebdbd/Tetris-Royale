#pragma once

#include "../include.hpp"
#include "utils.hpp"

class FriendList {
    std::vector<std::string>* listOfFriends;
    int numberOfFriends=0;
public:
    FriendList();
    void addFriend(std::string friendName);
    void removeFriend(std::string friendName);
    int getamountOfFriends();
    std::vector<std::string>* getFriends();
    ~FriendList();
};

class User {
    std::string userName;
    std::string password;
    int highScore=0;
    int matricule;
    FriendList* friendList;
    bool isOnline=false;
public:
    User(std::string userName, std::string password, int matricule=0);
    void addFriend(std::string friendName);
    bool login(std::string password);
    void logout();
    void setHighScore(int newHighScore);
    bool getIsOnline();
    int getHighScore();
    int getMatricule();
};

class Player : public User {
    int score=0;
public:
    Player(std::string userName, std::string password);
};

class Server {
    std::vector<User*>* users;
public:
    Server();
    void createUser(std::string userName, std::string password);
    // void addUser(User* user); ajouter un joueur dans une partie en tant que joueur ou spectateur
    void removeUser(User user);
    ~Server();
};
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class FriendList {
    vector<string> listOfFriends;
    int numberOfFriends=0;
public:
    void addFriend(string friendName);
    void removeFriend(string friendName);
    int getamountOfFriends();
};

class User {
    Server* server;
    string userName;
    string password;
    int highScore=0;
    int matricule;
    FriendList friendList;
    bool isOnline=false;
public:
    User(string userName, string password, int matricule);
    void addFriend(string friendName);
    void rename(string newName);
    int getHighScore();
    int getMatricule();
};

class Player : public User {};

class Server {
    vector<User*> users;
public:
    void createUser(string userName, string password);
    void addUser(User user);
    void removeUser(User user);
    void login(string userName, string password);
    void optimise(); // A voir comment on va faire pour rendre le vecteur plus petit
};
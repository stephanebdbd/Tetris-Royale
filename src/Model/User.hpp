#include <iostream>
#include <string>


class FriendList {
    string list[];
    int numberOfFriends=0;
public:
    addFriend(string friendName);
    removeFriend(string friendName);
    getamountOfFriends();
}

class User {
    string userName;
    string password;
    int highScore=0;
    FriendList friendList;
public:
    User(string userName, string password);
    addFriend(string friendName);
}

class Player : public User {}
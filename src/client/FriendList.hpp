#pragma once

#include "../include.hpp"
#include "User.hpp" // Inclusion de User.hpp pour pouvoir utiliser la classe User

class FriendList {
private:
    std::vector<std::shared_ptr<User>> friends;
    std::vector <std::shared_ptr<User>> friendRequests;
public:
    FriendList() = default;
    
    void addFriend(User* friendUser);
    void removeFriend(User* friendUser);
    void sendFriendRequest(std::string pseudoUser);
    void acceptFriendRequest(User* friendUser);
    void rejectFriendRequest(User* friendUser);
    void sendMessageToFriend(User* friendUser, std::string message);
    bool isFriend(User* friendUser);
    void removeFriendRequest(User* friendUser);
};

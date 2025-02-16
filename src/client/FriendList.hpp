#pragma once

#include "../include.hpp"
#include "User.hpp"

class FriendList {

private:

    std::vector<std::shared_ptr<User>> friends;
    std::vector <std::shared_ptr<User>> friendRequests;
    int friendRequestCount = 0;
    int friendCount = 0;

public:

    //Constructor
    FriendList() = default;

    //Getters
    const std::vector<std::shared_ptr<User>>& getFriends() const;
    const std::vector<std::shared_ptr<User>>& getFriendRequests() const;
    int getFriendRequestCount() const;
    int getFriendCount() const;

    //gerer la liste des amis
    void operator+=(std::shared_ptr<User> friendUser);
    void operator-=(std::shared_ptr<User> friendUser);

    //gerer les demandes d'amis
    void operator >> (std::shared_ptr<User> friendUser);
    void operator<<(std::shared_ptr<User> friendUser);
    
    //Destructor
    ~FriendList() = default;
};
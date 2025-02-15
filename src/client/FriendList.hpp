#pragma once

#include <vector>
#include <string>
#include <iostream>

class User; // Déclaration avant inclusion pour éviter la dépendance circulaire

class FriendList {
private:
    std::vector<User*> friends;
public:
    FriendList() = default;
    
    void addFriend(User* friendUser);
    void removeFriend(const std::string& friendName);
    int getAmountOfFriends() const;
    std::vector<User*> getFriends() const;
    void displayFriends() const;
};

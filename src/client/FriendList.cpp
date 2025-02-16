#include "FriendList.hpp"

FriendList::FriendList() = default;

const std::vector<std::shared_ptr<User>>& FriendList::getFriends() const {
    return friends;
}

//gerer la liste des amis
void FriendList::operator+=(std::shared_ptr<User> friendUser) {
    friends.emplace_back(friendUser);
    friendCount++;
}

void FriendList::operator-=(std::shared_ptr<User> friendUser) {
    for (int i = 0; i < friends.size(); i++) {
        if (friends[i] == friendUser) {
            friends.erase(friends.begin() + i);
            break;
        }
    }
    friendCount--;
}
//gerer les demandes d'amis
void FriendList::operator >> (std::shared_ptr<User> friendUser) {
    friendRequests.emplace_back(friendUser);
    friendRequestCount++;
}

void FriendList::operator<<(std::shared_ptr<User> friendUser) {
    for (int i = 0; i < friendRequests.size(); i++) {
        if (friendRequests[i] == friendUser) {
            friendRequests.erase(friendRequests.begin() + i);
            break;
        }
    }
    friendRequestCount--;
}
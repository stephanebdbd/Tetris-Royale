#include "FriendList.hpp"

void FriendList::addFriend(User* friendUser) {
    friends.emplace_back(friendUser);
}

void FriendList::removeFriend(User* friendUser) {
    for (int i = 0; i < friends.size(); i++) {
        if (friends[i] == friendUser) {
            friends.erase(friends.begin() + i);
            break;
        }
    }
}

void FriendList::sendFriendRequest(std::string pseudoUser) {
    //friendUser->receiveFriendRequest();
}

void FriendList::acceptFriendRequest(User* friendUser) {
    friends.emplace_back(friendUser);
    removeFriendRequest(friendUser);
}

void removeFriendRequest(User* friendUser) {
    for (int i = 0; i < friendRequests.size(); i++) {
        if (friendRequests[i] == friendUser) {
            friendRequests.erase(friendRequests.begin() + i);
            break;
        }
    }
}

bool FriendList::isFriend(User* friendUser) {
    for (int i = 0; i < friends.size(); i++) {
        if (friends[i] == friendUser) {
            return true;
        }
    }
    return false;
}

void FriendList::removeFriendRequest(User* friendUser) {
    for (int i = 0; i < friendRequests.size(); i++) {
        if (friendRequests[i] == friendUser) {
            friendRequests.erase(friendRequests.begin() + i);
            break;
        }
    }
}


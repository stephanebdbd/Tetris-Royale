#ifndef FRIENDLIST_HPP
#define FRIENDLIST_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

class FriendList {
private:
    std::unordered_map<std::string, std::vector<std::string>> friends;
    std::unordered_map<std::string, std::vector<std::string>> requests;
    std::string friendsFile;
    std::string requestsFile;

    // Chargement et sauvegarde des données
    void loadFriends();
    void saveFriends();
    void loadRequests();
    void saveRequests();

public:
    // Constructeurs
    FriendList();  // Constructeur par défaut
    FriendList(const std::string& friendsFilename, const std::string& requestsFilename);

    // Méthodes
    void registerUser(const std::string& username);
    void sendFriendRequest(const std::string& sender, const std::string& receiver);
    void acceptFriendRequest(const std::string& user, const std::string& friendToAccept);
    void rejectFriendRequest(const std::string& user, const std::string& friendToReject);
    bool areFriends(const std::string& user1, const std::string& user2) const;
    bool userExists(const std::string& userId) const;
    void removeFriend(const std::string& user, const std::string& friendToRemove);
    void removeFriendRequest(const std::string& sender, const std::string& receiver);
    std::vector<std::string> getFriendList(const std::string& user) const;
    std::vector<std::string> getRequestList(const std::string& user) const;
    bool isPendingRequest(const std::string& sender, const std::string& receiver) const;
};

#endif // FRIENDLIST_HPP

#ifndef FRIENDLIST_HPP
#define FRIENDLIST_HPP

#include <string>
#include <vector>
#include <unordered_map>

class FriendList {
private:
    std::unordered_map<std::string, std::vector<std::string>> friends;
    std::unordered_map<std::string, std::vector<std::string>> requests;
    std::string friendsFile ;  // Fichier pour stocker la liste des amis
    std::string requestsFile; // Fichier pour stocker les demandes d'amis

    // charggement des donnes depuis les fichier 
    std::unordered_map<std::string, std::vector<std::string>> loadFriends();
    void saveFriends();
    std::unordered_map<std::string, std::vector<std::string>> loadRequests();
    void saveRequests();

public:
    // Constructeur
    FriendList();


    void registerUser(const std::string& username);
    void sendFriendRequest(const std::string& sender, const std::string& receiver);
    void acceptFriendRequest(const std::string& user, const std::string& friendToAccept);
    void rejectFriendRequest(const std::string& user, const std::string& friendToReject);
    void listFriends(const std::string& user) const;
    void listPendingRequests(const std::string& user) const;
    bool areFriends(const std::string& user1, const std::string& user2) const;
    bool userExists(const std::string& userId) const;
    void removeFriend(const std::string& user, const std::string& friendToRemove);
    void removeFriendRequest(const std::string& sender, const std::string& receiver);
    std::vector<std::string> getFriendList(const std::string& user) const;
    std::vector<std::string> getRequestList(const std::string& user) const;
    bool isPendingRequest(const std::string& sender, const std::string& receiver);
    









};

#endif // FRIENDLIST_HPP
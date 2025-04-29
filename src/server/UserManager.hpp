#ifndef USERMANAGER_HPP
#define USERMANAGER_HPP

#include <string>
#include <vector>
#include <unordered_map>

struct UserData {
    std::string password;
    int highscore;
    int avatarIndex;
};

class UserManager {
    std::string file;
    std::unordered_map<std::string, UserData> users;

    void loadUsers();
    void saveUsers();

public:
    UserManager(const std::string& filename);
    bool userNotExists(const std::string& username) const;
    bool registerUser(const std::string& username, const std::string& password, int avatarIndex);
    bool authenticateUser(const std::string& username, const std::string& password);
    int getHighscore(const std::string& username);
    void updateHighscore(const std::string& username, int score);
    std::vector<std::pair<std::string, int>> getRanking() const;
    int getUserAvatar(const std::string& username) const {
        if (users.find(username) != users.end()) {
            return users.at(username).avatarIndex;
        }
        return -1; // Retourne -1 si aucun avatar n'est défini
    }
    void setUserAvatar(const std::string& username, int avatarIndex) {
        if (users.find(username) != users.end()) {
            users[username].avatarIndex = avatarIndex;
            saveUsers();
        }
    }
};

#endif
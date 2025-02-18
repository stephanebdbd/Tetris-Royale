#ifndef USERMANAGER_HPP
#define USERMANAGER_HPP

#include <string>
#include <unordered_map>

struct UserData {
    std::string password;
    int highscore;
};

class UserManager {
public:
    UserManager(const std::string& filename);
    bool registerUser(const std::string& username, const std::string& password);
    bool authenticateUser(const std::string& username, const std::string& password);
    int getHighscore(const std::string& username);
    void updateHighscore(const std::string& username, int score);

private:
    std::string file;
    std::unordered_map<std::string, UserData> users;

    void loadUsers();
    void saveUsers();
};

#endif

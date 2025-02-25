#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>

class Player {
    int clientId;
    std::string name;
    bool isConnected;
    bool isObserver;

public:
    Player(int clientId, const std::string& name, bool isObserver = false);
    int getId() const;
    std::string getName() const;
    bool getIsConnected() const;
    bool getIsObserver() const;
    void setIsConnected(bool status);
};

#endif
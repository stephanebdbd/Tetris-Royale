#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>

class Player {
    int clientId;
    int clientSocket;
    std::string name;
    bool isConnected;
    bool isObserver;

public:
    Player(int clientId, int clientSocket, const std::string& name, bool isObserver = false);
    int getId() const;
    std::string getName() const;
    bool getIsConnected() const;
    bool getIsObserver() const;
    int getPlayerSocket() const;
    void setIsConnected(bool status);
};

#endif
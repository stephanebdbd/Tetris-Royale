#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <memory>

class GameRoom;

class Player {
    int clientId;
    int clientSocket;
    std::string name;
    bool isObserver;
    std::shared_ptr<GameRoom> gameRoom;

public:
    Player(int clientId, int clientSocket, const std::string& name, bool isObserver = false, std::shared_ptr<GameRoom> gameRoom = nullptr);
    int getId() const;
    std::string getName() const;
    bool getIsObserver() const;
    int getPlayerSocket() const;
    void setIsConnected(bool status);
    bool sendMalusOrBonus(int targetId/*, int malusOrBonus*/);
    bool operator==(const Player& player) const;
    bool operator!=(const Player& player) const;
};

#endif
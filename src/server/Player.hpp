#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <memory>

class GameRoom;

class Player {
    int clientId;
    int playerId;
    int clientSocket;
    std::string name;
    std::shared_ptr<GameRoom> gameRoom;
    bool isObserver;

public:
    Player(int clientId, int playerId, int clientSocket, const std::string& name, GameRoom& gameRoom, bool isObserver = false);
    int getId() const;
    std::string getName() const;
    bool getIsObserver() const;
    int getPlayerSocket() const;
    void setPlayerId(int playerId) { this->playerId = playerId; }
    int getPlayerId() const { return playerId; }
    void setIsConnected(bool status);
    bool sendMalusOrBonus(int targetId/*, int malusOrBonus*/);
    bool operator==(const Player& player) const;
    bool operator!=(const Player& player) const;
};

#endif
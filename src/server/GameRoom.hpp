#ifndef GAMEROOM_HPP
#define GAMEROOM_HPP

#include <vector>
#include <string>
#include "Player.hpp"

class GameRoom {
    int roomId;
    std::string roomName;
    int maxPlayers;
    std::vector<Player> players;
    std::string gameMode;
    bool inProgress;

public:
    GameRoom(int id, const std::string& name, int maxPlayers, const std::string& gameMode);
    bool addPlayer(const Player& player);
    bool removePlayer(const Player& player);
    bool isFull() const;
    void startGame();
    void endGame();
    void setInProgress(bool status);
    bool getInProgress() const;
    int getRoomId() const;
    std::string getRoomName() const;
    std::string getGameMode() const;
    std::string setGameMode(const std::string& mode);
};

#endif
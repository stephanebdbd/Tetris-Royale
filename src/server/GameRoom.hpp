#ifndef GAMEROOM_HPP
#define GAMEROOM_HPP

#include <vector>
#include <string>
#include <iostream>
#include "Player.hpp"
#include "Game.hpp"
#include "GameMode.hpp"
#include "ClassicMode.hpp"
#include "DuelMode.hpp"
#include "EndlessMode.hpp"


class GameRoom {
    int roomId;
    int ownerId;
    int maxPlayers;
    int speed;
    GameMode gameMode;
    GameModeName gameModeName;
    std::unordered_map<int, int> clientsSockets;
    std::vector<Player> players;
    std::vector<Game> games;
    bool inProgress;

public:
    GameRoom(int roomId, int clientId, int maxPlayers, GameModeName gameMode);
    bool addPlayer(const Player& player);
    bool removePlayer(const Player& player);
    bool isFull() const;
    void startGame();
    void endGame();
    void applyFeatureMode(int clientId);
    void setInProgress(bool status);
    bool getInProgress() const;
    int getRoomId() const;
    int getOwnerId() const;
    void setOwnerId(int roomId);
    void setMaxPlayers(int max);
    int getMaxPlayers() const;
    GameMode getGameMode() const;
    void setGameMode(const std::string& mode);
};

#endif
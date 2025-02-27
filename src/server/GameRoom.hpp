#ifndef GAMEROOM_HPP
#define GAMEROOM_HPP

#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include <unistd.h>
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
    //std::vector<Player> viewers;
    bool inProgress;
    std::unordered_map<std::string, std::string> unicodeToText = {
        {"\u0005", "right"},
        {"\u0004", "left"},
        {"\u0003", "up"},
        {"\u0002", "down"},
        {" ", "drop"}
    };
public:
    GameRoom(int roomId, int clientId, int maxPlayers, GameModeName gameMode);
    bool addPlayer(const Player& player);
    bool removePlayer(const Player& player);
    bool isFull() const;
    void shiftPlayers(int index);
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
    void sendGameToPlayer(int PlayerId);
    void keyInputGameMenu(Player& player, const std::string& unicodeAction);
    std::string convertUnicodeToText(const std::string& unicode);
    GameMode getGameMode() const;
    void loopgame();
};

#endif
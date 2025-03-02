#ifndef GAMEROOM_HPP
#define GAMEROOM_HPP

#include <string>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <memory>
#include <sys/socket.h>
#include "Player.hpp"
#include "Game.hpp"
#include "GameMode.hpp"
#include "RoyalMode.hpp"
#include "ClassicMode.hpp"


class GameRoom {
    int roomId;
    int ownerId;
    int maxPlayers;
    GameModeName gameModeName;
    std::shared_ptr<GameMode> gameMode = nullptr;
    bool started=false;
    bool inProgress;
    int speed;
    int amountOfPlayers=0;
    std::vector<int> clientsSockets;
    std::vector<Player> players;
    std::vector<std::shared_ptr<Game>> games;
    std::vector<Player> viewers;
    std::unordered_map<std::string, std::string> unicodeToText = {
        {"\u0005", "right"},
        {"\u0004", "left"},
        {"\u0003", "up"},
        {"\u0002", "down"},
        {" ", "drop"}
    };
public:
    GameRoom()=default;
    GameRoom(int roomId, int clientId, int clientSocket, std::string pseudo, int maxPlayers, GameModeName gameModeName);
    void addPlayer(int clientId, int clientSocket, std::string pseudo);
    bool removePlayer(const Player& player);
    bool getIsFull() const;
    void shiftPlayers(int index);
    void startGame();
    void endGame();
    void applyFeatureMode(int clientId);
    void setInProgress(bool status);
    void setSpeed(int speed);
    bool setGameMode(GameModeName gameMode);
    void addViewer(const Player& player);
    bool getInProgress() const;
    int getRoomId() const;
    int getOwnerId() const;
    void setOwnerId(int roomId);
    void setMaxPlayers(int max);
    int getMaxPlayers() const;
    void sendGameToPlayer(int PlayerId);
    void keyInputGame(Player& player, const std::string& unicodeAction);
    std::string convertUnicodeToText(const std::string& unicode);
    void setInsanceGameMode();
    void handlePlayers(Player player);
    //void handleViewers();
    void loopgame();
    void setRoomId(int roomId) { this->roomId = roomId; }
    bool getHasStarted() const { return started; }
};

#endif
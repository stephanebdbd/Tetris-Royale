#ifndef GAMEROOM_HPP
#define GAMEROOM_HPP

#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <sys/socket.h>
#include "Game.hpp"
#include "GameMode.hpp"
#include "RoyalMode.hpp"
#include "ClassicMode.hpp"


class GameRoom {
    int roomId;
    int ownerId;
    GameModeName gameModeName;
    int maxPlayers;
    std::shared_ptr<GameMode> gameMode = nullptr;
    bool started=false;
    bool inProgress=false;
    int energyLimit;
    int speed;
    int amountOfPlayers=0;
    std::array<int, 9> energyOrClearedLines;
    std::array<int, 9> playersVictim;
    std::array<int, 9> playersMalusOrBonus;
    std::array<int, 9> players;
    std::array<std::shared_ptr<Game>, 9> games;

    std::vector<int> viewersId;
public:
    GameRoom()=default;
    GameRoom(int roomId, int clientId, GameModeName gameModeName=GameModeName::Endless, int maxPlayers=1);
    void addPlayer(int playerId);
    bool removePlayer(int playerId);
    bool getIsFull() const;
    void shiftPlayers(int index);
    void startGame();
    void handleMalusOrBonus(int playerId);
    void endGame();
    void applyFeatureMode(int playerId);
    void setInProgress(bool status);
    void setSpeed(int speed);
    bool setGameMode(GameModeName gameMode);
    void addViewer(int viewerId);
    bool getInProgress() const;
    int getRoomId() const;
    int getOwnerId() const;
    bool getGameIsOver(int playerServerId, bool fromGameRoom=false) const;
    void setOwnerId(int roomId);
    void setMaxPlayers(int max);
    int getMaxPlayers() const;
    bool getNeedToSendGame(int playerServerId) const;
    void setNeedToSendGame(bool needToSendGame, int playerServerId);
    void setInsanceGameMode();
    void setRoomId(int roomId) { this->roomId = roomId; }
    void setHasStarted();
    bool getHasStarted() const;
    void setGameIsOver(int playerServerId);
    void input(int playerId, const std::string& unicodeAction);
    GameModeName getGameModeName() const { return gameModeName; }
    int getAmountOfPlayers() const { return amountOfPlayers; }
    void inputLobby(const std::string& action);
    std::pair<std::string,int> extractNumber(const std::string& action);
    void keyInputGame(int playerId, const std::string& unicodeAction);
    void keyInputchooseVictim(int playerId, int victim);
    void keyInputchooseMalusorBonus(int playerId, int malusOrBonus);
    void reinitializeMalusOrBonus(int playerId);
    std::shared_ptr<Game> getGame(int playerServerId);
    std::string convertUnicodeToText(const std::string& unicode);
    int convertStringToInt(const std::string& unicodeAction);
    bool getCanUseMalusOrBonus(int playerServerId) const;
    std::shared_ptr<Score> getScore(int playerServerId) const;
    int getPlayerId(int playerServerId) const;

    std::array<int, 9> getPlayers() const { return players; }
};

#endif

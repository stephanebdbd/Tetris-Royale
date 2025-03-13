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
    bool started=false;
    bool inProgress=false;
    int energyLimit=25;
    int speed=0;
    int gameModeIndex=-1;
    int amountOfPlayers=0;
    std::vector<std::shared_ptr<GameMode>> gameModes;
    std::vector<int> energyOrClearedLines;
    std::vector<int> playersVictim;
    std::vector<int> playersMalusOrBonus;
    std::vector<int> players;
    std::vector<Game> games;
    std::vector<Score> scores;

    std::vector<int> viewersId;

public:
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
    void setGameMode(GameModeName gameMode);
    void addViewer(int viewerId);
    bool getInProgress() const;
    int getRoomId() const;
    int getOwnerId() const;
    bool getGameIsOver(int playerServerId, bool fromGameRoom=false) const;
    void setMaxPlayers(int max);
    int getMaxPlayers() const;
    bool getNeedToSendGame(int playerServerId) const;
    void setNeedToSendGame(bool needToSendGame, int playerServerId);
    void setRoomId(int roomId) { this->roomId = roomId; }
    void setHasStarted();
    bool getHasStarted() const;
    bool getSettingsDone() const;
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
    Game& getGame(int playerServerId);
    std::string convertUnicodeToText(const std::string& unicode);
    int convertStringToInt(const std::string& unicodeAction);
    bool getCanUseMalusOrBonus(int playerServerId) const;
    Score& getScore(int playerServerId);
    int getPlayerId(int playerServerId) const;
    void setEnergyLimit(int NewEnergyLimit);
    int getEnergyLimit() const;
    int getScoreValue(int playerServerId) const;
    bool verifyVectors() const;

    std::vector<int> getPlayers() const { return players; }
    int getSpeed() const { return speed; }

    bool readyToPlay = false;
};

#endif

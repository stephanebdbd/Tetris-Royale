#ifndef GAMEROOM_HPP
#define GAMEROOM_HPP

#include <string>
#include <iostream>
#include <thread>
#include <unordered_map>
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
    bool inProgress;
    int energyLimit;
    int speed;
    int amountOfPlayers=0;
    std::vector<int> energyOrClearedLines;
    std::vector<int> playersVictim;
    std::vector<int> playersMalusOrBonus;
    std::vector<int> players;
    std::vector<std::shared_ptr<Game>> games;

    std::vector<int> viewersId;
    std::unordered_map<std::string, std::string> unicodeToText = {
        {"\u0005", "right"},
        {"\u0004", "left"},
        {"\u0003", "up"},
        {"\u0002", "down"},
        {" ", "drop"}
    };
    std::unordered_map<std::string, int> stringToIntegers = {
        {"1", 1}, {"2", 2}, {"3", 3},
        {"4", 4}, {"5", 5}, {"6", 6},
        {"7", 7}, {"8", 8}, {"9", 9}
    };

public:
    GameRoom()=default;
    GameRoom(int roomId, int clientId, GameModeName gameModeName, int maxPlayers=1);
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
    //GameModeName getGameModeName() const {return gameModeName;}
    //int getAmountOfPlayers() const {return amountOfPlayers;}
    int trouverIndice(std::vector<int> vec, int valeur) const;


    void addViewer(int viewerId);
    bool getInProgress() const;
    int getRoomId() const;
    int getOwnerId() const;
    //bool getGameIsOver(int playerServerId) const { return games[players[playerServerId]]->getIsGameOver(); }
    bool getGameIsOver(int playerServerId) const { return games[trouverIndice(players, playerServerId)]->getIsGameOver(); }

    void setOwnerId(int roomId);
    void setMaxPlayers(int max);
    int getMaxPlayers() const;
    bool getNeedToSendGame(int playerId) const { return games[trouverIndice(players, playerId)]->getNeedToSendGame(); }
    void setNeedToSendGame(bool needToSendGame, int playerId) { games[trouverIndice(players, playerId)]->setNeedToSendGame(needToSendGame); }
    void setInsanceGameMode();
    void setRoomId(int roomId) { this->roomId = roomId; }
    void setHasStarted();
    bool getHasStarted() const { return started; }
    void setGameIsOver(int playerServerId) { games[trouverIndice(players, playerServerId)]->setGameOver(); }
    void input(int playerId, const std::string& unicodeAction);
    GameModeName getGameModeName() const { return gameModeName; }
    int getAmountOfPlayers() const { return amountOfPlayers; }
    void inputLobby(const std::string& action);
    std::pair<std::string,int> extractNumber(const std::string& action);
    void keyInputGame(int playerId, const std::string& unicodeAction);
    void keyInputchooseVictim(int playerId, int victim);
    void keyInputchooseMalusorBonus(int playerId, int malusOrBonus);
    void reinitializeMalusOrBonus(int playerId);
    std::shared_ptr<Game> getGame(int playerId) { return games[trouverIndice(players, playerId)]; }
    std::string convertUnicodeToText(const std::string& unicode);
    int convertStringToInt(const std::string& unicodeAction);
    bool getCanUseMalusOrBonus(int PlayerServerId) const;
    Score getScore(int PlayerServerId) const { return games[trouverIndice(players, PlayerServerId)]->getScore(); }
};

#endif

#ifndef GAMEROOM_HPP
#define GAMEROOM_HPP

#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include <sys/socket.h>
#include "Game.hpp"
#include "GameMode.hpp"
#include "RoyalMode.hpp"
#include "ClassicMode.hpp"
#include "../common/jsonKeys.hpp"


class GameRoom {
    int roomId;
    int ownerId;
    GameModeName gameModeName;
    int maxPlayers = 1;
    bool inProgress = false;
    bool ownerQuit = false;
    int energyLimit=0;
    int speed=0;
    int gameModeIndex=-1;
    int amountOfPlayers=0;
    std::vector<std::shared_ptr<GameMode>> gameModes;
    std::vector<int> energyOrClearedLines;
    std::vector<int> playersVictim;
    std::vector<int> playersMalusOrBonus;
    std::vector<int> players;
    std::vector<std::shared_ptr<Game>> games;
    std::vector<int> viewersId;
    std::map<int, int> observerCurrentPlayer;

    using MessageMap = std::unordered_map<std::string, bool>;
    MessageMap Cmessages = {
    {jsonKeys::PROPOSITION_CIBLE, false},
    {jsonKeys::CHOICE_CIBLE, false},
    {jsonKeys::CHOICE_MALUS_BONUS, false},  
    {jsonKeys::CHOICE_MALUS, false},  
    {jsonKeys::CHOICE_BONUS, false},
    {jsonKeys::CLEAR, false},
    {jsonKeys::GAME_OVER, false}
}; 
    mutable std::vector<MessageMap> messageList;
    std::vector<std::string> boolInputs = {"Y", "y", "N", "n"};
    int victimRandom = -1;
    std::vector<bool> applyMalus;
    std::vector<bool> showmessage;
    mutable std::vector<int> keyClear;
public:
    GameRoom(int roomId, int clientId, GameModeName gameModeName);
    void addPlayer(int playerId);
    bool removePlayer(int playerId);
    bool getIsFull() const;
    void shiftPlayers(int index);
    void handleMalusOrBonus(int playerId);
    void startGame();
    void endGame();
    void applyFeatureMode(int playerId);
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
    bool getSettingsDone() const;
    void input(int playerServerId, const std::string& unicodeAction,std::string status);
    GameModeName getGameModeName() const { return gameModeName; }
    int getAmountOfPlayers() const { return amountOfPlayers; }
    void setAmountOfPlayers(int amount);
    void inputLobby(int clientId, const std::string& action);
    void keyInputGame(int playerId, const std::string& unicodeAction);
    void keyInputchooseVictim(int playerId, int victim);
    void keyInputchooseMalusorBonus(int playerId, int malusOrBonus);
    void reinitializeMalusOrBonus(int playerId);
    std::shared_ptr<Game> getGame(int playerServerId);
    std::string convertUnicodeToText(const std::string& unicode);
    int convertSettingToInt(const std::string& unicodeAction, std::size_t length);
    int convertStringToInt(const std::string& unicodeAction);
    bool getCanUseMalusOrBonus(int playerServerId) const;
    int getScoreValue() const;
    int getPlayerId(int playerServerId) const;
    void setEnergyLimit(int NewEnergyLimit);
    int getEnergyLimit() const;
    void setOwnerQuit();
    bool getOwnerQuit() const;
    int getGameModeIndex() const { return gameModeIndex; }
    void createGames();
    int getEnergyOfPlayer(int playerServerId) const;
    void updatePlayerGame(int playerServerId);

    std::vector<int> getPlayers() const { return players; }
    int getSpeed() const { return speed; }
    json messageToJson(int playerServerId)const;

    void choiceVictimRandomly(int playerId);
    void observerNextPlayer(int observerId);
    void observerPrevPlayer(int observerId);
    std::vector<int> getViewers() {return viewersId;}

    int getclientobserverId(int observerId) {
        if (observerCurrentPlayer.find(observerId) != observerCurrentPlayer.end()) {
            int observedPlayerIndex = observerCurrentPlayer[observerId];
            if (observedPlayerIndex >= 0 && observedPlayerIndex < static_cast<int>(games.size())) {
                std::cout << "Client ID: " << players[observedPlayerIndex] << std::endl;
            }
            return observedPlayerIndex;
        }
        return -1;
    }
};

#endif
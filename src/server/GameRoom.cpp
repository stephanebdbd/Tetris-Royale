#include "GameRoom.hpp"
#include <cstring>
#include <unistd.h>


GameRoom::GameRoom(int roomId, int clientId, GameModeName gameModeName, int maxPlayers)
    : roomId(roomId), ownerId(clientId), gameModeName(gameModeName), maxPlayers(maxPlayers),
    inProgress(false) {
    setGameMode(gameModeName);
    if ((gameModeName == GameModeName::Endless) && (maxPlayers != 1))
    maxPlayers = 1;
    else if ((gameModeName == GameModeName::Duel) && (maxPlayers != 2))
        maxPlayers = 2;
    else if (((gameModeName == GameModeName::Royal_Competition) || (gameModeName == GameModeName::Classic)) && (maxPlayers > 2))
        maxPlayers = 3;
    this->addPlayer(clientId);
    std::cout << "GameRoom #" << roomId << " created." << std::endl;
    std::thread gameRoomThread(&GameRoom::startGame, this);
    gameRoomThread.detach();
    }

void GameRoom::addPlayer(int playerId) {
    int currentAmount = players.size();
    if (currentAmount < maxPlayers) {
        players.push_back(playerId);
        games.push_back(std::make_shared<Game>(10, 20));
        energyOrClearedLines.push_back(0);
        playersVictim.push_back(-1);
        playersMalusOrBonus.push_back(-1);
    }
    amountOfPlayers++;
}

bool GameRoom::removePlayer(int playerId) {
    auto it = std::find(players.begin(), players.end(), playerId);
    if (it != players.end()) {
        int index = std::distance(players.begin(), it);
        players.erase(players.begin() + index);
        games.erase(games.begin() + index);
        energyOrClearedLines.erase(energyOrClearedLines.begin() + index);
        playersVictim.erase(playersVictim.begin() + index);
        playersMalusOrBonus.erase(playersMalusOrBonus.begin() + index);
        amountOfPlayers--;
        if ((index < amountOfPlayers-1) && (amountOfPlayers > 1))
            this->shiftPlayers(index);
        return true;
    }
    return false;
}

bool GameRoom::getIsFull() const {
    return amountOfPlayers == maxPlayers;
}

void GameRoom::setHasStarted(){
    started = true;
    inProgress = true;
}

void GameRoom::startGame() {
    while(!this->getIsFull())
        continue;

    if (gameModeName == GameModeName::Duel) {
        playersVictim[0] = 1;
        playersVictim[1] = 0;
    }
    
    this->setHasStarted();
    int countGameOvers = 0;
    
    while (inProgress) {
        countGameOvers = 0;
        for (int i = 0; i < maxPlayers; ++i) {
            if (this->getGameIsOver(players[i]))
                countGameOvers++;
            else {
                games[i]->updateGame();
                if (gameMode != nullptr) this->handleMalusOrBonus(i);
            }
            amountOfPlayers = maxPlayers - countGameOvers;
        }
        if ((countGameOvers == maxPlayers-1) && (gameModeName != GameModeName::Endless)) 
            this->endGame();
        else if ((countGameOvers == 1) == (gameModeName == GameModeName::Endless))
            this->endGame();
    }
}

void GameRoom::handleMalusOrBonus(int playerId) {
    if (gameModeName == GameModeName::Royal_Competition) {
        energyOrClearedLines[playerId] += games[playerId]->getLinesCleared();;
        if ((energyOrClearedLines[playerId] >= energyLimit) && (playersMalusOrBonus[playerId] != -1) && (playersVictim[playerId] != -1)) {
            this->applyFeatureMode(playerId);
            this->reinitializeMalusOrBonus(playerId);
        }
    }
    else if ((gameModeName == GameModeName::Duel) || (gameModeName == GameModeName::Classic)) {
        playersMalusOrBonus[playerId] = games[playerId]->getLinesCleared();
        if (playersMalusOrBonus[playerId] > 1) {
            if (gameModeName == GameModeName::Duel){
                this->applyFeatureMode(playerId);
                this->reinitializeMalusOrBonus(playerId);
            }
            else if (playersVictim[playerId] != -1) {
                this->applyFeatureMode(playerId);
                this->reinitializeMalusOrBonus(playerId);
            } 
        }
    }
}

void GameRoom::reinitializeMalusOrBonus(int playerId) {
    energyOrClearedLines[playerId] = 0;
    playersMalusOrBonus[playerId] = -1;
    playersVictim[playerId] = -1;
}

bool GameRoom::getCanUseMalusOrBonus(int playerId) const {
    if (gameModeName == GameModeName::Royal_Competition) {
        return energyOrClearedLines[playerId] >= energyLimit;
    }
    else if ((gameModeName == GameModeName::Duel) || (gameModeName == GameModeName::Classic)) {
        return playersMalusOrBonus[playerId] > 1;
    }
    return false;
}

void GameRoom::keyInputchooseMalusorBonus(int playerId, int malusOrBonus) {
    if (gameModeName == GameModeName::Royal_Competition){
        if ((malusOrBonus > 0) && (malusOrBonus < 10))
            playersMalusOrBonus[playerId] = malusOrBonus;
    } 
}

void GameRoom::keyInputchooseVictim(int playerId, int victim) {
    if ((gameModeName == GameModeName::Classic) || (gameModeName == GameModeName::Royal_Competition)){
        if (victim <= maxPlayers)
            playersVictim[playerId] = victim-1;
    }
}

void GameRoom::endGame() {
    inProgress = false;
}

void GameRoom::applyFeatureMode(int playerId) {
    int victim = playersVictim[playerId];
    //int malusOrBonus = playersMalusOrBonus[playerId];
    gameMode->featureMode(games[victim]/*, malusOrBonus*/);
}

void GameRoom::setInProgress(bool status) { this->inProgress = status; }

void GameRoom::setSpeed(int speed) { this->speed = speed; }

bool GameRoom::setGameMode(GameModeName gameMode) {
    if ((amountOfPlayers == 1) && (gameModeName == GameModeName::Endless)) {
        this->gameModeName = gameMode;
        return true;
    }
    if ((amountOfPlayers <= 2) && (gameModeName == GameModeName::Duel)) {
        this->gameModeName = gameMode;
        return true;
    }
    if ((gameModeName == GameModeName::Classic) || (gameModeName == GameModeName::Royal_Competition)) {
        this->gameModeName = gameMode;
        return true;
    }
    return false;
}

void GameRoom::addViewer(int viewerId) {
    viewersId.push_back(viewerId);
}

bool GameRoom::getInProgress() const { return inProgress; }

int GameRoom::getRoomId() const { return roomId; }

void GameRoom::setInsanceGameMode() {
    switch (gameModeName) {
    case GameModeName::Classic:
        this->gameMode = std::make_shared<ClassicMode>();
        break;
    case GameModeName::Duel:
        this->gameMode = std::make_shared<ClassicMode>();
        break;
    case GameModeName::Endless:
        this->gameMode = nullptr;
        break;
    case GameModeName::Royal_Competition:
        this->gameMode = std::make_shared<RoyalMode>();
        break;
    default:
        break;
    }
}

void GameRoom::setOwnerId(int clientId) { ownerId = clientId; }

int GameRoom::getOwnerId() const { return ownerId; }

void GameRoom::setMaxPlayers(int max) { maxPlayers = max; }

int GameRoom::getMaxPlayers() const { return maxPlayers; }

void GameRoom::shiftPlayers(int index) {
    for (int i = index; i < maxPlayers - 1; ++i) {
        players[i] = players[i + 1];
        games[i] = games[i + 1];
        energyOrClearedLines[i] = energyOrClearedLines[i + 1];
        playersVictim[i] = playersVictim[i + 1];
        playersMalusOrBonus[i] = playersMalusOrBonus[i + 1];
    }
    players.pop_back();
    games.pop_back();
    energyOrClearedLines.pop_back();
    playersVictim.pop_back();
    playersMalusOrBonus.pop_back();
}

std::string GameRoom::convertUnicodeToText(const std::string& unicodeAction) {
    auto action = unicodeToText.find(unicodeAction);
    return (action != unicodeToText.end()) ? action->second : "///"; 
}

int GameRoom::convertStringToInt(const std::string& unicodeAction) {
    auto action = stringToIntegers.find(unicodeAction);
    return (action != stringToIntegers.end()) ? action->second : -1;
}

void GameRoom::keyInputGame(int playerId, const std::string& unicodeAction) {
    std::string action = convertUnicodeToText(unicodeAction);
    games[playerId]->moveTetramino(action);
}

void GameRoom::input(int PlayerServerId, const std::string& unicodeAction) {
    int playerId = players[PlayerServerId];
    if (this->getIsFull()) {
        if (this->getCanUseMalusOrBonus(playerId)) {
            int action = this->convertStringToInt(unicodeAction);
            if (action != -1) {
                if ((playersMalusOrBonus[playerId] == -1) && (gameModeName == GameModeName::Royal_Competition))
                this->keyInputchooseMalusorBonus(playerId, action);
                else
                this->keyInputchooseVictim(playerId, action);
            }
        }
        else
            this->keyInputGame(playerId, unicodeAction);
    }
    /*else if (amountOfPlayers < maxPlayers)
        this->inputLobby(playerId, unicodeAction);
    */
}
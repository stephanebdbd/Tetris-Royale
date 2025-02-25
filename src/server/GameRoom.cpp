#include "GameRoom.hpp"

GameRoom::GameRoom(int roomId, int clientId, int maxPlayers, GameModeName gameModeName)
    : roomId(roomId), ownerId(clientId), maxPlayers(maxPlayers), gameModeName(gameModeName), gameMode{this->getGameMode()}, inProgress(false) {}

bool GameRoom::addPlayer(const Player& player) {
    if (players.size() < maxPlayers) {
        players.push_back(player);
        return true;
    }
    return false;
}

bool GameRoom::removePlayer(const Player& player) {
    auto it = std::find(players.begin(), players.end(), player);
    if (it != players.end()) {
        players.erase(it);
        return true;
    }
    return false;
}

bool GameRoom::isFull() const {
    return players.size() == maxPlayers;
}

void GameRoom::startGame() {
    inProgress = true;
    // ...
}

void GameRoom::endGame() {
    inProgress = false;
    // ...
}

void GameRoom::setInProgress(bool status) { inProgress = status; }

bool GameRoom::getInProgress() const { return inProgress; }

int GameRoom::getRoomId() const { return roomId; }

void GameRoom::applyFeatureMode(int clientId) { /*gameMode->feautureMode(&games[clientId]);*/ }

GameMode GameRoom::getGameMode() const {
    switch (gameModeName) {
    case GameModeName::Classic:
        return ClassicMode();
        break;
    case GameModeName::Duel:
        return DuelMode();
        break;
    case GameModeName::Endless:
        return EndlessMode();
        break;
    case GameModeName::Royal_Competition:
        //return RoyalMode();
        break;
    default:
        break;
    }
}
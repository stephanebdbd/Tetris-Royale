#include "GameRoom.hpp"

GameRoom::GameRoom(int id, const std::string& name, int maxPlayers, const std::string& gameMode)
    : roomId(id), roomName(name), maxPlayers(maxPlayers), gameMode(gameMode), inProgress(false) {}

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

std::string GameRoom::getRoomName() const { return roomName; }

std::string GameRoom::getGameMode() const { return gameMode; }

void GameRoom::setGameMode(const std::string& mode) { gameMode = mode; }
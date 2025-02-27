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
    int it = std::find(players.begin(), players.end(), player);
    if (it != players.end()) {
        players.erase(it);
        games.erase(it);
        clientsSockets.erase(it);
        if (it != maxPlayers - 1) {
            this->shiftPlayers(it);
        }
        maxPlayers--;
        return true;
    }
    return false;
}

bool GameRoom::isFull() const {
    return players.size() == maxPlayers;
}

void GameRoom::startGame() {
    inProgress = true;
    this->sendGameToPlayers();
    this->loopgame();
}

void GameRoom::endGame() {
    inProgress = false;
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

void GameRoom::setOwnerId(int clientId) { ownerId = clientId; }

int GameRoom::getOwnerId() const { return ownerId; }

void GameRoom::setMaxPlayers(int max) { maxPlayers = max; }

int GameRoom::getMaxPlayers() const { return maxPlayers; }

void GameRoom::sendGameToPlayer(int playerId) {
    auto& game = games[playerId];

    json message;
    
    message["score"] = game.getScore().scoreToJson();
    message["grid"] = game.getGrid().gridToJson();
    message["tetraPiece"] = game.getCurrentPiece().tetraminoToJson(); // Ajout du tétrimino dans le même message

    std::string msg = message.dump() + "\n";
    send(players[playerId].getPlayerSocket(), msg.c_str(), msg.size(), 0); // Un seul envoi
}

void GameRoom::loopgame() {
    this->startGame();
    for (int playerId = 0; playerId < maxPlayers; ++playerId) {
        auto& game = games[playerId];
        std::thread gameThread([this, playerId]() { // Lancer un thread pour mettre à jour le jeu
            auto& gameInstance = games[playerId];
            auto& displacement = games[playerId].getDisplacement();
            while (inProgress) {
                displacement.timerHandler();
                if (!gameInstance.getNeedToSendGame()) {
                    this->sendGameToPlayer(playerId);
                    gameInstance.setNeedToSendGame(false);
                }
                    
            }
        });
        gameThread.detach();
    }
    this->endGame();
}

void GameRoom::shiftPlayers(int index) {
    players.erase(maxPlayers - 1);
    games.erase(maxPlayers - 1);
    clientsSockets.erase(maxPlayers - 1);
    for (int i = index; i < maxPlayers - 1; ++i) {
        players[i] = players[i + 1];
        games[i] = games[i + 1];
        clientsSockets[i] = clientsSockets[i + 1];
    }
}

std::string GameRoom::convertUnicodeToText(const std::string& unicode) {
    auto action = unicodeToText.find(unicode);
    return (action != unicodeToText.end()) ? action->second : "///"; 
}
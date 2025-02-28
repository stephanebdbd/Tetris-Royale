#include "GameRoom.hpp"

GameRoom::GameRoom(int roomId, int clientId, int maxPlayers, GameModeName gameModeName)
    : roomId(roomId), ownerId(clientId), maxPlayers(maxPlayers), gameModeName(gameModeName),
    inProgress(false) {
    setGameMode(gameModeName);
    }

bool GameRoom::addPlayer(const Player& player) {
    int currentAmount = players.size();
    if (currentAmount < maxPlayers) {
        players.push_back(player);
        amountOfPlayers++;
        games.push_back(Game(10, 20));
        clientsSockets[amountOfPlayers] = player.getPlayerSocket();
        return true;
    }
    return false;
}

bool GameRoom::removePlayer(const Player& player) {
    auto it = std::find(players.begin(), players.end(), player);
    if (it != players.end()) {
        int index = std::distance(players.begin(), it);
        players.erase(it);
        games.erase(games.begin() + index);
        clientsSockets.erase(index);
        int currentAmount = players.size();
        if (currentAmount != maxPlayers - 1) {
            this->shiftPlayers(index);
        }
        amountOfPlayers--;
        return true;
    }
    return false;
}

bool GameRoom::isFull() const {
    return amountOfPlayers == maxPlayers;
}

void GameRoom::startGame() {
    inProgress = true;
    for (int playerId = 0; playerId < maxPlayers; ++playerId)
        this->sendGameToPlayer(playerId);
    this->loopgame();
}

void GameRoom::endGame() {
    inProgress = false;
}

void GameRoom::applyFeatureMode(int clientId) {
    if (gameMode != nullptr)
        gameMode->featureMode(games[clientId]);
}

void GameRoom::setInProgress(bool status) { this->inProgress = status; }

void GameRoom::setSpeed(int speed) { this->speed = speed; }

bool GameRoom::setGameMode(GameModeName gameMode) {
    if ((amountOfPlayers == 1) && (gameMode == GameModeName::Endless)) {
        this->gameModeName = gameMode;
        return true;
    }
    if ((amountOfPlayers <= 2) && (gameMode == GameModeName::Duel)) {
        this->gameModeName = gameMode;
        return true;
    }
    if ((gameMode == GameModeName::Classic) || (gameMode == GameModeName::Royal_Competition)) {
        this->gameModeName = gameMode;
        return true;
    }
    return false;
}

void GameRoom::addViewer(const Player& player) {
    viewers.push_back(player);
}

bool GameRoom::getInProgress() const { return inProgress; }

int GameRoom::getRoomId() const { return roomId; }

void GameRoom::getGameMode() {
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

void GameRoom::sendGameToPlayer(int playerId) {
    auto& game = games[playerId];
    auto& displacement = game.getDisplacement();

    json message;
    
    message["score"] = game.getScore().scoreToJson();
    message["grid"] = game.getGrid().gridToJson();
    message["tetraPiece"] = displacement.getCurrentPiece().tetraminoToJson(); // Ajout du tétrimino dans le même message

    std::string msg = message.dump() + "\n";
    send(players[playerId].getPlayerSocket(), msg.c_str(), msg.size(), 0); // Un seul envoi
}

void GameRoom::loopgame() {
    this->startGame();
    for (int playerId = 0; playerId < maxPlayers; ++playerId) {
        std::thread gameThread([this, playerId]() { // Lancer un thread pour mettre à jour le jeu
            auto& gameInstance = games[playerId];
            auto& displacement = gameInstance.getDisplacement();
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
    for (int i = index; i < maxPlayers - 1; ++i) {
        players[i] = players[i + 1];
        games[i] = games[i + 1];
        clientsSockets[i] = clientsSockets[i + 1];
    }
    int clientIndex = std::distance(players.begin(), players.end());
    players.pop_back();
    games.pop_back();
    clientsSockets.erase(clientIndex);

}

std::string GameRoom::convertUnicodeToText(const std::string& unicode) {
    auto action = unicodeToText.find(unicode);
    return (action != unicodeToText.end()) ? action->second : "///"; 
}
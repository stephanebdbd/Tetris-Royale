#include "GameRoom.hpp"
#include <cstring>
#include <unistd.h>


GameRoom::GameRoom(int roomId, int clientId, int clientSocket, std::string pseudo, int maxPlayers, GameModeName gameModeName)
    : roomId(roomId), ownerId(clientId), maxPlayers(maxPlayers), gameModeName(gameModeName),
    inProgress(false) {
    setGameMode(gameModeName);
    this->addPlayer(clientId, clientSocket, pseudo);
    std::thread gameRoomThread(&GameRoom::loopgame, this);
    gameRoomThread.detach();
    }

void GameRoom::addPlayer(int clientId, int clientSocket, std::string pseudo) {
    Player player(clientId, amountOfPlayers, clientSocket, pseudo, *this, false);
    int currentAmount = players.size();
    if (currentAmount < maxPlayers) {
        players.push_back(player);
        games.push_back(std::make_shared<Game>(10, 20));
        clientsSockets.push_back(player.getPlayerSocket());
        std::thread playerThread(&GameRoom::handlePlayers, this, player);
        playerThread.detach();
    }
    amountOfPlayers++;
}

bool GameRoom::removePlayer(const Player& player) {
    auto it = std::find(players.begin(), players.end(), player);
    if (it != players.end()) {
        int index = std::distance(players.begin(), it);
        players.erase(it);
        games.erase(games.begin() + index);
        close(player.getPlayerSocket());
        clientsSockets.erase(clientsSockets.begin() + index);
        int currentAmount = players.size();
        if (currentAmount != maxPlayers - 1) {
            this->shiftPlayers(index);
        }
        amountOfPlayers--;
        return true;
    }
    return false;
}

bool GameRoom::getIsFull() const {
    return amountOfPlayers == maxPlayers;
}

void GameRoom::startGame() {
    inProgress = true;
    for (int playerId = 0; playerId < maxPlayers; ++playerId)
        this->sendGameToPlayer(playerId);
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

void GameRoom::sendGameToPlayer(int playerId) {
    auto& game = games[playerId];

    json message;
    
    message["score"] = game->getScore().scoreToJson();
    message["grid"] = game->getGrid().gridToJson();
    message["tetraPiece"] = game->getCurrentPiece().tetraminoToJson(); // Ajout du tétrimino dans le même message

    std::string msg = message.dump() + "\n";
    send(players[playerId].getPlayerSocket(), msg.c_str(), msg.size(), 0); // Un seul envoi
}

void GameRoom::loopgame() {
    while (!this->getIsFull()){
        continue; // Attendre que la salle soit pleine
    }
    std::cout << "GameRoom #" << roomId << " créée." << std::endl;
    this->startGame();
    started = true;
    for (int playerId = 0; playerId < maxPlayers; ++playerId) {
        std::thread gameThread([this, playerId]() { // Lancer un thread pour mettre à jour le jeu
            std::shared_ptr<Game> gameInstance = games[playerId];
            while (true) {
                gameInstance->timerHandler();
                if (gameInstance->getNeedToSendGame()) {
                    std::cout << "Envoi du jeu au joueur " << playerId << std::endl;
                    this->sendGameToPlayer(playerId);
                    gameInstance->setNeedToSendGame(false);
                }
                if (gameInstance->getIsGameOver())
                    break;       
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
        players[i].setPlayerId(i);
    }
    players.pop_back();
    games.pop_back();
    clientsSockets.pop_back();
}

std::string GameRoom::convertUnicodeToText(const std::string& unicode) {
    auto action = unicodeToText.find(unicode);
    return (action != unicodeToText.end()) ? action->second : "///"; 
}

void GameRoom::handlePlayers(Player player) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(player.getPlayerSocket(), buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            std::cout << "coucou" << std::endl;
            try {
                json receivedData = json::parse(buffer);
                std::string action = receivedData["action"];

                std::cout << "Action reçue du client " << player.getPlayerId() << " : " << action << std::endl;
                this->keyInputGame(player, action);
            } 
            catch (json::parse_error& e) {
                std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
            }
        }
    }
}

void GameRoom::keyInputGame(Player& player, const std::string& unicodeAction) {
    std::string action = convertUnicodeToText(unicodeAction);
    games[player.getPlayerId()]->moveTetramino(action);
}
#include "GameRoom.hpp"
#include <cstring>
#include <unistd.h>


GameRoom::GameRoom(int roomId, int clientId, GameModeName gameModeName, int maxPlayers)
    : roomId(roomId), ownerId(clientId), gameModeName(gameModeName), maxPlayers(maxPlayers) {
    gameModes.push_back(std::make_shared<ClassicMode>());
    gameModes.push_back(std::make_shared<RoyalMode>());

    setGameMode(gameModeName);
    if ((gameModeName == GameModeName::Endless) && (maxPlayers != 1)){
        maxPlayers = 1;
    }
        
    else if ((gameModeName == GameModeName::Duel) && (maxPlayers != 2)){
        maxPlayers = 2;

        std::cout << "maxPlayers = " <<maxPlayers<<std::endl;
    }
        
    else if (((gameModeName == GameModeName::Royal_Competition) || (gameModeName == GameModeName::Classic)) && (maxPlayers < 2)){
        maxPlayers = 3;
    }
        
    addPlayer(clientId);
    std::cout << "GameRoom #" << roomId << " created." << std::endl;
}

void GameRoom::addPlayer(int playerId) {
    if (amountOfPlayers < maxPlayers) {
        players.push_back(playerId);
        energyOrClearedLines.push_back(0);
        playersVictim.push_back(-1);
        playersMalusOrBonus.push_back(-1);
        amountOfPlayers++;
    }
}

bool GameRoom::removePlayer(int playerId) {
    for (int idx=0; idx < amountOfPlayers ; idx++) {
        if (players[idx] == playerId) {
            amountOfPlayers--;
            players.erase(players.begin() + idx);
            energyOrClearedLines.erase(energyOrClearedLines.begin() + idx);
            playersVictim.erase(playersVictim.begin() + idx);
            playersMalusOrBonus.erase(playersMalusOrBonus.begin() + idx);
            return true;
        }
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

bool GameRoom::getHasStarted() const {
    return started;
}

bool GameRoom::getSettingsDone() const {
    bool done = (getSpeed() > 100) && getIsFull();
    if (gameModeName == GameModeName::Royal_Competition)
        done = done && (energyLimit < 15);
    return done;

}

void GameRoom::startGame() {

    while(!getIsFull()) continue;

    if (gameModeName == GameModeName::Duel) {
        playersVictim.resize(maxPlayers);
        playersVictim[0] = 1;
        playersVictim[1] = 0;
    }

    for (int idx = 0; idx < maxPlayers; idx++) {
        games.push_back(Game(10, 20, getSpeed()));
    }

    readyToPlay = true;
    setHasStarted();

    inProgress = true;
    
    int countGameOvers = 0;
    while (inProgress) {
        countGameOvers = 0;

        for (int i = 0; i < maxPlayers; ++i) {

            if (getGameIsOver(i, true)) {
                countGameOvers++;
            }
            else {
                games[i].updateGame();
                if (gameModeName != GameModeName::Endless)
                    handleMalusOrBonus(i);
            }

            amountOfPlayers = maxPlayers - countGameOvers;
        }

        // Conditions de fin
        if ((countGameOvers == maxPlayers - 1) && (gameModeName != GameModeName::Endless)) {
            endGame();
        } else if ((countGameOvers == 1) == (gameModeName == GameModeName::Endless)) {
            endGame();
        }
    }
}


void GameRoom::handleMalusOrBonus(int playerId) {
    if (gameModeName == GameModeName::Royal_Competition) {
        energyOrClearedLines[playerId] += games[playerId].getLinesCleared();;
        if ((energyOrClearedLines[playerId] >= energyLimit) && (playersMalusOrBonus[playerId] != -1) && (playersVictim[playerId] != -1)) {
            applyFeatureMode(playerId);
            reinitializeMalusOrBonus(playerId);
        }
    }
    else if ((gameModeName == GameModeName::Duel) || (gameModeName == GameModeName::Classic)) {
        playersMalusOrBonus[playerId] = games[playerId].getLinesCleared();
        if (playersMalusOrBonus[playerId] > 1) {
            if (gameModeName == GameModeName::Duel){
                applyFeatureMode(playerId);
                reinitializeMalusOrBonus(playerId);
            }
            else if (playersVictim[playerId] != -1) {
                applyFeatureMode(playerId);
                reinitializeMalusOrBonus(playerId);
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
    if (gameModeName != GameModeName::Endless){
        int victim = playersVictim[playerId];
        std::cout << "victim = " << victim << std::endl;
        //int malusOrBonus = playersMalusOrBonus[playerId];
        gameModes[gameModeIndex]->featureMode(games[victim]/*, malusOrBonus*/);
    }
}

void GameRoom::setInProgress(bool status) {
    inProgress = status;
}

void GameRoom::setSpeed(int newSpeed) {
    if (newSpeed > 100)
        speed = newSpeed;    
}

void GameRoom::setGameMode(GameModeName gameMode) {
    if (gameModeName != gameMode)
        gameModeName = gameMode;
    if (gameModeName == GameModeName::Endless) {
        gameModeIndex = -1;
        setMaxPlayers(1);
    }
    else if (gameModeName == GameModeName::Royal_Competition) {
        gameModeIndex = 1;
        if (maxPlayers < 3)
            setMaxPlayers(3);
    }
    else {
        gameModeIndex = 0;
        if (maxPlayers < 2)
            setMaxPlayers((gameModeName == GameModeName::Duel) ? 2 : 3);
    }
}

void GameRoom::addViewer(int viewerId) {
    viewersId.push_back(viewerId);
}

bool GameRoom::getInProgress() const {
    return inProgress;
}

int GameRoom::getRoomId() const { return roomId; }

void GameRoom::setOwnerId(int clientId) { ownerId = clientId; }

int GameRoom::getOwnerId() const { return ownerId; }

void GameRoom::setMaxPlayers(int max) { maxPlayers = max; }

int GameRoom::getMaxPlayers() const { return maxPlayers; }

std::string GameRoom::convertUnicodeToText(const std::string& unicodeAction) {
    switch (unicodeAction[0]) {
        case '\u0005':
            return "right";
        case '\u0004':
            return "left";
        case '\u0003':
            return "up";
        case '\u0002':
            return "down";
        case ' ':
            return "drop";
        default:
            break;
        }
    return "///";
}

int GameRoom::convertStringToInt(const std::string& unicodeAction) {
    int action = -1;
    try {
        action = std::stoi(unicodeAction);
    }
    catch (std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
    }
    return ((action > 0) && (action < 10)) ? action : -1;
}

void GameRoom::keyInputGame(int playerId, const std::string& unicodeAction) {
    std::string action = convertUnicodeToText(unicodeAction);
    games[playerId].moveTetramino(action);
}

void GameRoom::input(int PlayerServerId, const std::string& unicodeAction) {
    int playerId = getPlayerId(PlayerServerId);
    if (playerId == -1)
        return;
    if (getHasStarted()) {
        if (getCanUseMalusOrBonus(playerId)) {
            int action = convertStringToInt(unicodeAction);
            if (action != -1) {
                if ((playersMalusOrBonus[playerId] == -1) && (gameModeName == GameModeName::Royal_Competition))
                    keyInputchooseMalusorBonus(playerId, action);
                else
                    keyInputchooseVictim(playerId, action);
            }
        }
        else
            keyInputGame(playerId, unicodeAction);
    }
    /*else if (amountOfPlayers < maxPlayers)
        inputLobby(playerId, unicodeAction);
    */
}

std::pair<std::string,int> GameRoom::extractNumber(const std::string& action){
    /*size_t pos_parametre = action.find(' ');
    std::string parametre = action.substr(0, pos_parametre);
    size_t pos_number = pos_parametre + 2;
    std::string number = action.substr(pos_number);
    return {parametre, std::stoi(number)};*/

    if (action.rfind("\\speed", 0) == 0) { // Vérifie si l'action commence par "\speed"
        size_t pos_number = 6; // La position du premier caractère après "\speed"
        std::string number = action.substr(pos_number);
        return {"Speed", std::stoi(number)};
    }
    return {"", 0};


}

void GameRoom::inputLobby(const std::string& action){
    if (action.rfind("\\invite", 0) != 0){
        int number = std::stoi(action.substr(1));
        setMaxPlayers(number);
    }
    
    /*auto [parametre, number] = extractNumber(action);
    std::cout<<"hello"<<std::endl;
    if(parametre == "Speed"){
        std::cout<<"Speed : "<<number<<std::endl;
        setSpeed(number);
        games[playerId].setSpeed(speed);

    }else if(parametre == "MaxPlayers"){
        setMaxPlayers(number);
    }else if(parametre == "EnergyLimit"){
        setEnergyLimit(number);
    }*/
}

std::optional<Score> GameRoom::getScore(int playerServerId) {
    int playerId = getPlayerId(playerServerId);
    return games[playerId].getScore();
}

Game& GameRoom::getGame(int playerServerId) {
    int playerId = getPlayerId(playerServerId);
    return games[playerId];
}

void GameRoom::setGameIsOver(int playerServerId) {
    int playerId = getPlayerId(playerServerId);
    if ((playerId != -1) && (playerId < amountOfPlayers))
    games[playerId].setGameOver();
}

void GameRoom::setNeedToSendGame(bool needToSendGame, int playerServerId) {
    int playerId = getPlayerId(playerServerId);
    if ((playerId != -1) && (playerId < amountOfPlayers))
    games[playerId].setNeedToSendGame(needToSendGame);
}

bool GameRoom::getNeedToSendGame(int playerServerId) const {
    int playerId = getPlayerId(playerServerId);
    return games[playerId].getNeedToSendGame();
}

int GameRoom::getPlayerId(int playerServerId) const {
    if (amountOfPlayers == 0)
        return -1;
    if ((amountOfPlayers == 1) || (players[0] == playerServerId))
        return 0;
    for (int i = 1; i < amountOfPlayers; ++i) {
        if (players[i] == playerServerId)
            return i;
    }
    return -1;
}

bool GameRoom::getGameIsOver(int playerServerId, bool fromGameRoom) const {
    int playerId = (fromGameRoom) ? playerServerId : getPlayerId(playerServerId);
    return games[playerId].getIsGameOver();
}

int GameRoom::getScoreValue(int playerServerId) const {
    int playerId = getPlayerId(playerServerId);
    std::optional<Score> score = games[playerId].getScore();
    return score->getScore();
}

void GameRoom::setEnergyLimit(int newEnergyLimit) {
    if (newEnergyLimit < 15)
        energyLimit = newEnergyLimit;
}

int GameRoom::getEnergyLimit() const {
    return energyLimit;
}
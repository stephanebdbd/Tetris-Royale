#include "GameRoom.hpp"
#include <cstring>
#include <unistd.h>


GameRoom::GameRoom(int roomId, int clientId, GameModeName gameModeName, int maxPlayers)
    : roomId(roomId), ownerId(clientId), gameModeName(gameModeName), maxPlayers(maxPlayers) {
    
    gameModes.emplace_back(std::make_shared<ClassicMode>());
    gameModes.emplace_back(std::make_shared<RoyalMode>());

    setGameMode(gameModeName);
    if (gameModeName == GameModeName::Endless)
        setSpeed(1000);

    if ((gameModeName == GameModeName::Endless) && (maxPlayers != 1)){
        this->maxPlayers = 1;
    }
        
    else if ((gameModeName == GameModeName::Duel) && (maxPlayers != 2)){
        this->maxPlayers = 2;
    }
        
    else if (((gameModeName == GameModeName::Royal_Competition) || (gameModeName == GameModeName::Classic)) && (maxPlayers < 2)){
        this->maxPlayers = 3;
    }
    addPlayer(clientId);
    std::cout << "Max players: " << maxPlayers << std::endl;
    std::cout << "GameRoom #" << roomId << " created at " << this << std::endl;
}

void GameRoom::addPlayer(int playerId) {
    if (getAmountOfPlayers() < getMaxPlayers()) {
        players.push_back(playerId);
        energyOrClearedLines.push_back(0);
        playersVictim.push_back(-1);
        playersMalusOrBonus.push_back(-1);
        std::cout << "Joueur #" << players[getAmountOfPlayers()] << " ajouté dans la GameRoom " << getRoomId() << std::endl;
        amountOfPlayers++;
    }
}

bool GameRoom::removePlayer(int playerId) {
    for (int idx=0; idx < getMaxPlayers() ; idx++) {
        if (players[idx] == playerId) {
            amountOfPlayers--;
            if (idx != getAmountOfPlayers())
                shiftPlayers(idx);
            players.pop_back();
            energyOrClearedLines.pop_back();
            playersVictim.pop_back();
            playersMalusOrBonus.pop_back();
            return true;
        }
    }
    return false;
}

void GameRoom::shiftPlayers(int index){
    for (int i = index; i < getMaxPlayers() - 1; ++i)
        players[i] = players[i + 1];
}

bool GameRoom::getIsFull() const {
    return getAmountOfPlayers() == getMaxPlayers();
}

void GameRoom::setToStartGame(){
    started = true;
    inProgress = true;
}

bool GameRoom::getHasStarted() const {
    return started;
}

bool GameRoom::getSettingsDone() const {
    bool done = (getSpeed() > 100) && getIsFull();
    if (getGameModeName() == GameModeName::Royal_Competition)
        done = done && (energyLimit < 15);
    return done;

}

void GameRoom::startGame() {
    std::cout << "Let's create GameRoom #" << roomId << " at " << this << std::endl;

    if (getGameModeName() != GameModeName::Endless) {
        while(!(getSettingsDone() || getOwnerQuit())) continue;

        if (getOwnerQuit()) {
            endGame();
            return;
        }

        if (getGameModeName() == GameModeName::Duel) {
            playersVictim[0] = 1;
            playersVictim[1] = 0;
        }
    }

    int speed = getSpeed();

    for (int idx = 0; idx < getMaxPlayers(); idx++){
        games.emplace_back(std::make_shared<Game>(10, 20, speed));
        std::cout << "Game #" << idx << " created at " << &games[idx] << std::endl;
    }

    setCanGetGames();

    while (!getHasStarted()) continue;

    int countGameOvers = 0;
    
    if (getGameModeName() != GameModeName::Endless)
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    setCanPlay();

    while (getInProgress()) {
    
        countGameOvers = 0;

        for (int i = 0; i < getMaxPlayers(); ++i) {

            if (getGameIsOver(i, true))
                countGameOvers++;

            else {
                games[i]->updateGame();
                if (getGameModeName() != GameModeName::Endless)
                    handleMalusOrBonus(i);
            }

            amountOfPlayers = getMaxPlayers() - countGameOvers;
        }

        // Conditions de fin
        if ((countGameOvers == getMaxPlayers() - 1) && (getGameModeName() != GameModeName::Endless))
            endGame();
        else if ((countGameOvers == 1) && (getGameModeName() == GameModeName::Endless))
            endGame();
    }
}


void GameRoom::handleMalusOrBonus(int playerId) {
    if (getGameModeName() == GameModeName::Royal_Competition) {
        energyOrClearedLines[playerId] += games[playerId]->getLinesCleared();;
        if ((energyOrClearedLines[playerId] >= energyLimit) && (playersMalusOrBonus[playerId] != -1) && (playersVictim[playerId] != -1)) {
            applyFeatureMode(playerId);
            reinitializeMalusOrBonus(playerId);
        }
    }
    else if ((getGameModeName() == GameModeName::Duel) || (getGameModeName() == GameModeName::Classic)) {
        playersMalusOrBonus[playerId] = games[playerId]->getLinesCleared();
        if (playersMalusOrBonus[playerId] > 1) {
            if (getGameModeName() == GameModeName::Duel){
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
    if (getGameModeName() == GameModeName::Royal_Competition) {
        return energyOrClearedLines[playerId] >= energyLimit;
    }
    else if ((getGameModeName() == GameModeName::Duel) || (getGameModeName() == GameModeName::Classic)) {
        return playersMalusOrBonus[playerId] > 1;
    }
    return false;
}

void GameRoom::keyInputchooseMalusorBonus(int playerId, int malusOrBonus) {
    if (getGameModeName() == GameModeName::Royal_Competition){
        if ((malusOrBonus > 0) && (malusOrBonus < 8)){
            playersMalusOrBonus[playerId] = malusOrBonus;
            if (malusOrBonus > 5)
                playersVictim[playerId] = playerId; // Bonus appliqué sur soi-même
        }
    } 
}

void GameRoom::keyInputchooseVictim(int playerId, int victim) {
    if ((getGameModeName() == GameModeName::Classic) || (getGameModeName() == GameModeName::Royal_Competition)){
        if (victim <= getMaxPlayers())
            playersVictim[playerId] = victim-1;
    }
}

void GameRoom::endGame() {
    inProgress = false;
}

void GameRoom::applyFeatureMode(int playerId) {
    if (getGameModeName() != GameModeName::Endless){
        int victim = playersVictim[playerId];
        int malusOrBonus = playersMalusOrBonus[playerId];
        gameModes[getGameModeIndex()]->featureMode(games[victim], malusOrBonus);
    }
}

void GameRoom::setInProgress(bool status) {
    inProgress = status;
}

void GameRoom::setSpeed(int newSpeed) {
    std::cout << "Speed: " << newSpeed << std::endl;
    if (newSpeed > 100)
        this->speed = newSpeed;    
}

void GameRoom::setGameMode(GameModeName gameMode) {
    if (getGameModeName() != gameMode)
        gameModeName = gameMode;
    if (getGameModeName() == GameModeName::Endless) {
        gameModeIndex = -1;
        setMaxPlayers(1);
    }
    else if (getGameModeName() == GameModeName::Royal_Competition) {
        gameModeIndex = 1;
        if (getMaxPlayers() < 3)
            setMaxPlayers(3);
    }
    else {
        gameModeIndex = 0;
        if (getMaxPlayers() < 2)
            setMaxPlayers((getGameModeName() == GameModeName::Duel) ? 2 : 3);
    }
}

void GameRoom::addViewer(int viewerId) {
    viewersId.push_back(viewerId);
}

bool GameRoom::getInProgress() const {
    return inProgress;
}

int GameRoom::getRoomId() const { return roomId; }

int GameRoom::getOwnerId() const { return ownerId; }

int GameRoom::getMaxPlayers() const { return maxPlayers; }

void GameRoom::setOwnerQuit() { ownerQuit = true; }

bool GameRoom::getOwnerQuit() const { return ownerQuit; }

void GameRoom::setCanGetGames() { canGetGames = true; }

bool GameRoom::getCanGetGames() const { return canGetGames; }

void GameRoom::setCanPlay() { canPlay = true; }

bool GameRoom::getCanPlay() const { return canPlay; }

void GameRoom::setMaxPlayers(int max) {
    if ((getGameModeName() != GameModeName::Endless) && (getGameModeName() != GameModeName::Duel)){
        if ((max > 2) && (max < 10) && (getAmountOfPlayers() <= max))
            maxPlayers = max;    
    }
}

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
    catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
    }
    return ((action > 0) && (action < 10)) ? action : -1;
}

void GameRoom::keyInputGame(int playerId, const std::string& unicodeAction) {
    std::string action = convertUnicodeToText(unicodeAction);
    games[playerId]->moveTetramino(action);
}

void GameRoom::input(int playerServerId, const std::string& unicodeAction) {
    std::cout << "GameRoom #" << roomId << " received input from player #" << playerServerId << ": " << unicodeAction << std::endl;
    if (!getSettingsDone())
        inputLobby(playerServerId, unicodeAction);
    int playerId = getPlayerId(playerServerId);
    if ((playerId == -1) || getOwnerQuit())
        return;
    if (getHasStarted()) {
        if (getCanUseMalusOrBonus(playerId)) {
            int action = convertStringToInt(unicodeAction);
            if (action != -1) {
                if ((playersMalusOrBonus[playerId] == -1) && (getGameModeName() == GameModeName::Royal_Competition))
                    keyInputchooseMalusorBonus(playerId, action);
                else
                    keyInputchooseVictim(playerId, action);
            }
        }
        else
            keyInputGame(playerId, unicodeAction);
    }
    
}

int GameRoom::convertSettingToInt(const std::string& unicodeAction, std::size_t length){
    int action = -1; length++;
    try {
        std::size_t unicodeActionLength = unicodeAction.length();
        if (unicodeActionLength > length)
            action = std::stoi(unicodeAction.substr(length, unicodeActionLength));
        else
            std::cerr << "Erreur: Pas de valeur entrée" << std::endl;
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
    }
    catch (const std::out_of_range& e) {
        std::cerr << "Out of range: " << e.what() << std::endl;
    }
    return action;
}


void GameRoom::inputLobby(int clientId, const std::string& action){
    if (action.rfind("/max", 0) == 0){
        std::size_t length = action.find('/', std::string("/max").size()); // Trouver le premier \ après \max
        if (length == std::string::npos) // Si le format est incorrect, retourner sans faire rien
            return;
        int number = convertSettingToInt(action, length);
        if ((number > 0) && (clientId == ownerId))
            this->setMaxPlayers(number);
    }
    else if(action.rfind("/speed", 0) == 0){
        std::size_t length = action.find('/', std::string("/speed").size()); // Trouver le premier \ après \speed
        int number = convertSettingToInt(action, length);
        if (length == std::string::npos) // Si le format est incorrect, retourner sans faire rien
            return;
        if ((number > 0) && (clientId == ownerId))
            this->setSpeed(number);
    }
    else if (action.rfind("/energy", 0) == 0) {
        std::size_t length = action.find('/', std::string("/energy").size()); // Trouver le premier \ après \energy
        if (length == std::string::npos) // Si le format est incorrect, retourner sans faire rien
            return;        
        int number = convertSettingToInt(action, length);
        if ((number > 0) && (clientId == ownerId))
            this->setEnergyLimit(number);
    }
}

void GameRoom::setOwnerId(int clientId) { ownerId = clientId; }

Score& GameRoom::getScore(int playerServerId) {
    int playerId = getPlayerId(playerServerId);
    return games[playerId]->getScore();
}

std::shared_ptr<Game> GameRoom::getGame(int playerServerId) {
    int playerId = getPlayerId(playerServerId);

    if (playerId < 0 || playerId >= static_cast<int>(games.size())) {
        std::cerr << "Error: Invalid player ID: " << playerId << std::endl;
        return nullptr;  
    }

    std::cout << "Game #" << playerId << " at : " << games[playerId].get() << std::endl;
    return games[playerId];
}

void GameRoom::setGameIsOver(int playerServerId) {
    std::cout << "GameRoom #" << roomId << " received game over from player #" << playerServerId << std::endl;
    int playerId = getPlayerId(playerServerId);
    if ((playerId != -1) && (playerId < getAmountOfPlayers()))
    games[playerId]->setGameOver();
}

void GameRoom::setNeedToSendGame(bool needToSendGame, int playerServerId) {
    int playerId = getPlayerId(playerServerId);
    if ((playerId != -1) && (playerId < getAmountOfPlayers()))
    games[playerId]->setNeedToSendGame(needToSendGame);
}

bool GameRoom::getNeedToSendGame(int playerServerId) const {
    int playerId = getPlayerId(playerServerId);
    if (games[playerId]->getNeedToSendGame())
        std::cout << "Player #" << playerId << " needs to get game." << std::endl;
    return games[playerId]->getNeedToSendGame();
}

int GameRoom::getPlayerId(int playerServerId) const {
    if ((getGameModeName() == GameModeName::Endless) || (players[0] == playerServerId))
        return 0;
    for (int i = 1; i < getMaxPlayers(); ++i) {
        if (players[i] == playerServerId)
            return i;
    }
    std::cerr << "Erreur: Joueur " << playerServerId << " non trouvé dans le vecteur players." << std::endl;
    return -1;
}

bool GameRoom::getGameIsOver(int playerServerId, bool fromGameRoom) const {
    int playerId = (fromGameRoom) ? playerServerId : getPlayerId(playerServerId);
    int gamesSize = games.size();
    if ((playerId < 0 ) || (playerId >= gamesSize)) {
        std::cerr << "Erreur: Tentative d'accès à un index invalide dans `games` : " << playerId << " et gamesSize : " << gamesSize << std::endl;
        return true;
    }
    return games[playerId]->getIsGameOver();
}

void GameRoom::setEnergyLimit(int newEnergyLimit) {
    if (newEnergyLimit < 15)
        energyLimit = newEnergyLimit;
}

int GameRoom::getEnergyLimit() const {
    return energyLimit;
}

int GameRoom::getEnergyOfPlayer(int playerServerId) const {
    int playerId = getPlayerId(playerServerId);
    return energyOrClearedLines[playerId];
}
#include "Player.hpp"
#include "GameRoom.hpp"

Player::Player(int clientId, int playerId, int clientSocket, const std::string& name, GameRoom& gameRoom, bool isObserver)
    : clientId(clientId), playerId(playerId), clientSocket(clientSocket),name(name), gameRoom{std::make_shared<GameRoom>(gameRoom)}, isObserver(isObserver) {}

int Player::getId() const { return clientId; }
std::string Player::getName() const { return name; }
bool Player::getIsObserver() const { return isObserver; }
int Player::getPlayerSocket() const { return clientSocket; }
bool Player::sendMalusOrBonus(int targetId/*, int malusOrBonus*/) {
    if ((gameRoom != nullptr) && (gameRoom->getInProgress()) && (targetId != clientId) && (targetId < gameRoom->getMaxPlayers()) && (!this->isObserver)){
        gameRoom->applyFeatureMode(targetId/*, malusOrBonus*/); // implémenter le fait qu'on puisse identifier le malus ou le bonus
        return true;
    }
    return false;
}
bool Player::operator==(const Player& player) const { return clientId == player.clientId; }
bool Player::operator!=(const Player& player) const { return clientId != player.clientId; }
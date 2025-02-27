#include "Player.hpp"

Player::Player(int clientId, int clientSocket, const std::string& name, bool isObserver)
    : clientId(clientId), clientSocket(clientSocket),name(name), isConnected(true), isObserver(isObserver) {}

int Player::getId() const { return clientId; }
std::string Player::getName() const { return name; }
bool Player::getIsConnected() const { return isConnected; }
bool Player::getIsObserver() const { return isObserver; }
void Player::setIsConnected(bool status) { isConnected = status; }
int Player::getPlayerSocket() const { return clientSocket; }
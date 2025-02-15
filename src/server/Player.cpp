#include "Player.hpp"

Player::Player(int id, const std::string& name, bool isObserver)
    : id(id), name(name), isConnected(true) {}

int Player::getId() const { return id; }
std::string Player::getName() const { return name; }
bool Player::getIsConnected() const { return isConnected; }
void Player::setIsConnected(bool status) { isConnected = status; }
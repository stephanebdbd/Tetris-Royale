#include "Player.hpp"

Player::Player(int id, const std::string& name, bool isObserver)
    : id(id), name(name), isConnected(true), isObserver(isObserver) {}

int Player::getId() const { return id; }
std::string Player::getName() const { return name; }
bool Player::getIsConnected() const { return isConnected; }
bool Player::getIsObserver() const { return isObserver; }
void Player::setIsConnected(bool status) { isConnected = status; }
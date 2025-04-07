#include "Bonus.hpp"

Bonus::Bonus(std::shared_ptr<Game> game) : game(game) {
}

void Bonus::miniBlock(){
    game->applyMiniTetraminoBonus();
}

void Bonus::decreaseSpeed(){
    game->setSpeedBonusMalus(true);
    game->applySpeedBonusMalus(1300);
}
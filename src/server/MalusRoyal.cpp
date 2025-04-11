#include "MalusRoyal.hpp"


MalusRoyal::MalusRoyal(std::shared_ptr<Game> game): game(game){}

void MalusRoyal::reverseControl(){
    
    game->setCommandIsReversed(true);

}

void MalusRoyal::increaseSpeed(){
    game->applySpeedBonusMalus(-650);
    game->setSpeedBonusMalus(true);

}
void MalusRoyal::blockControl(){
    game->setBlockCommand(true);
}
void MalusRoyal::turnOffLight(){
    game->setlightBlocked(true);
}   
void MalusRoyal::clear2x2Block(){
    game->random2x2MaskedBlock();
}
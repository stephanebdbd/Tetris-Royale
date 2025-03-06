#include "Bonus.hpp"

Bonus::Bonus(std::shared_ptr<TetraminoDisplacement> displacement) : displacement(displacement){}

void Bonus::MiniBlock(){
    std::array<std::array<char, 4>, 4> miniBlock = {{
        {' ', ' ', ' ', ' '},
        {' ', '#', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }};

    
    displacement->setCurrentPiece(miniBlock);

}

void Bonus::decreaseSpeed(){
    displacement->setBonus1Royal(true);
    displacement->setSpeed(-30);
}
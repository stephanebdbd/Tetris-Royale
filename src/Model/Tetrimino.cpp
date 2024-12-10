#include "Tetrimino.hpp"

Tetrimino::Tetrimino(TetriminoType type) : type(type) { 
    chooseColor();
}

std::vector<Position> Tetrimino::getBlocks(TetriminoType type) {
    switch (type) {
        case TetriminoType::Z:
            return {Position{0,1}, Position{1,1}, Position{1,2}, Position{2,2}};
        case TetriminoType::L:
            return {Position{0, 2}, Position{1, 2}, Position{2,2}, Position{2,1}};
        case TetriminoType::O:
            return {Position{1,1}, Position{1,2}, Position{2,1}, Position{2,2}};
        case TetriminoType::S:
            return {Position{0,2}, Position{1,2}, Position{1,1}, Position{2,1}};
        case TetriminoType::I:
            return {Position{0,1}, Position{1,1}, Position{2,1}, Position{3,1}};
        case TetriminoType::J:
            return {Position{0,1}, Position{0,2}, Position{1,2}, Position{2,2}};
        case TetriminoType::T:
            return {Position{0,1}, Position{1,1}, Position{2,1}, Position{1,2}};
        default:
            return {};
    }
}

void Tetrimino::chooseColor() {
    switch (type) {
        case TetriminoType::I:
            colour = Colour::BLUE;
            break;
        case TetriminoType::O:
            colour = Colour::YELLOW;
            break;
        case TetriminoType::T:
            colour = Colour::PURPLE;
            break;
        case TetriminoType::S:
            colour = Colour::GREEN;
            break;
        case TetriminoType::Z:
            colour = Colour::RED;
            break;
        case TetriminoType::L:
            colour = Colour::ORANGE;
            break;
        case TetriminoType::J:
            colour = Colour::BROWN;
            break;
    }
}

Colour Tetrimino::getColour() {
    return colour;
}

TetriminoType Tetrimino::getType() {
    return type;
}
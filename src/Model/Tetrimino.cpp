#include <iostream>
#include <Grid.hpp>

Tetrimino::Tetrimino(TetriminoType type) : type(type), rotation(0) {
    switch (type) {
        case TetriminoType::I:
            blocks = {Position{0,1}, Position{1,1}, Position{2,1}, Position{3,1}};
            colour = Colour::BLUE;
            break;
        case TetriminoType::O:
            blocks = {Position{1,1}, Position{1,2}, Position{2,1}, Position{2,2}};
            colour = Colour::YELLOW;
            break;
        case TetriminoType::T:
            blocks = {Position{0,1}, Position{1,1}, Position{2,1}, Position{1,2}};
            colour = Colour::PURPLE;
            break;
        case TetriminoType::S:
            blocks = {Position{0,2}, Position{1,2}, Position{1,1}, Position{2,1}};
            colour = Colour::GREEN;
            break;
        case TetriminoType::Z:
            blocks = {Position{0,1}, Position{1,1}, Position{1,2}, Position{2,2}};
            colour = Colour::RED;
            break;
        case TetriminoType::L:
            blocks = {Position{0, 2}, Position{1, 2}, Position{2,2}, Position{2,1}};
            colour = Colour::ORANGE;
            break;
        case TetriminoType::J:
            blocks = {Position{0, 1}, Position{0,2}, Position{1,2}, Position{3, 2}};
            colour = Colour::BROWN;
            break;
    }
}
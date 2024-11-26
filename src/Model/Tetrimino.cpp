#include <iostream>
#include <Grid.hpp>

Tetrimino::Tetrimino(TetriminoType type, Position upperLeft, vector<vector<Cell*>>* grid) : type(type), upperLeft(upperLeft), grid(grid) {
    array<Position, 4> blocks = setTetriminoBlocks(type);
    setColour(type, blocks);
}

array<Position, 4> Tetrimino::setTetriminoBlocks(TetriminoType type) {
    switch (type) {
        case TetriminoType::I:
            return {Position{0,1}, Position{1,1}, Position{2,1}, Position{3,1}};
        case TetriminoType::O:
            return {Position{1,1}, Position{1,2}, Position{2,1}, Position{2,2}};
        case TetriminoType::T:
            return {Position{0,1}, Position{1,1}, Position{2,1}, Position{1,2}};
        case TetriminoType::S:
            return {Position{0,2}, Position{1,2}, Position{1,1}, Position{2,1}};
        case TetriminoType::Z:
            return {Position{0,1}, Position{1,1}, Position{1,2}, Position{2,2}};
        case TetriminoType::L:
            return {Position{0, 2}, Position{1, 2}, Position{2,2}, Position{2,1}};
        case TetriminoType::J:
            return {Position{0, 1}, Position{0,2}, Position{1,2}, Position{3, 2}};
    }
    
}

Colour Tetrimino::setColour(TetriminoType type, array<Position, 4> blocks) {
    Colour colour;
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
    for (Position blockPosition : blocks) {
        (*grid)[blockPosition.y + upperLeft.y][blockPosition.x + upperLeft.x]->setColour(colour);
    }
}

void Tetrimino::rotate(bool clockwise) {
    if (type != TetriminoType::O) { // Si ce n'est pas un carré
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (clockwise)
                    setPosition(Position{x, 3-y}, Position{y, x}); // trouver un moyen d'expliquer "3"
                else
                    setPosition(Position{3-x, y}, Position{y, x}); // trouver un moyen d'expliquer "3"
            }     
        }
    }
}

void Tetrimino::setPosition(Position position1, Position position2) {
    int x1 = position1.x + upperLeft.x;
    int y1 = position1.y + upperLeft.y;
    int x2 = position2.x + upperLeft.x;
    int y2 = position2.y + upperLeft.y;
    (*grid)[y1][x1]->setPosition(x2, y2);
    (*grid)[y2][x2]->setPosition(x1, y1);
    Cell* tmp = (*grid)[y1][x1];
    (*grid)[y1][x1] = (*grid)[y2][x2];
    (*grid)[y2][x2] = tmp;
}


Colour Tetrimino::getColour() {
    return colour;
}
    
    
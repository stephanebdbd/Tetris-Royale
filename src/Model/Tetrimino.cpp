#include <iostream>
#include <Grid.hpp>

Tetrimino::Tetrimino(TetriminoType type, Position upperLeft, Grid* grid) : type(type), upperLeft(upperLeft), grid(grid), gridMatrix(grid->getGrid()) { 
    vector<Position> blocks = setTetriminoBlocks(type);
    setColour(type, blocks);

}

vector<Position> Tetrimino::setTetriminoBlocks(TetriminoType type) {
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
    }
    return {};
}


Colour Tetrimino::setColour(TetriminoType type, vector<Position> blocks) {
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
        (*gridMatrix)[blockPosition.y + upperLeft.y][blockPosition.x + upperLeft.x]->setColour(colour);
    }
}

void Tetrimino::rotate(bool clockwise) {
    vector<Position> blocksPositions;
    int count = 0, x = 0, y = 0;
    Position position = upperLeft, position2 = Position{0, 0};

    while ((x < amountBlocks) && (count < amountBlocks)) {
        position = upperLeft; y = 0;
        while ((y < amountBlocks) && (count < amountBlocks)) {
            position.x+=x; position.y+=y;
            if (clockwise) 
                position2 = Position{upperLeft.x + boxDimension - y, upperLeft.y + x};
            else
                position2 = Position{upperLeft.x + boxDimension - y, upperLeft.y + x};    
            count += checkColoration(position, position2, (&blocksPositions));
            y++;
        }
        if (count != amountBlocks) x++;
    }

    for (x; x < amountBlocks; x++){
        for (y; y < amountBlocks; y++){
            (*gridMatrix)[y + upperLeft.y][x + upperLeft.x]->setdefaultColour();
        }
    }

    colorate(blocksPositions);
}


int Tetrimino::checkColoration(Position position, Position position2, vector<Position>* blocksPositions) {
    if ((*gridMatrix)[position.y][position.x]->getIsColoured()) {
        (*gridMatrix)[position.y][position.x]->setdefaultColour();
        if (!(*gridMatrix)[position2.y][position2.x]->getIsColoured())
            blocksPositions->push_back(position2);
        return 1;
    }
    return 0;
}

void Tetrimino::colorate(vector<Position> blocks){
    for (auto block : blocks) {
        (*gridMatrix)[block.y + upperLeft.y][block.x + upperLeft.x]->setColour(colour);
    }
}

Colour Tetrimino::getColour() {
    return colour;
}

#include <Grid.hpp>


Grid::Grid(){
    for(int y=0; y<height; y++){
        for (int x=0; x<width; x++){
            gridMatrix[y][x] = new Cell(x, y);
        }
    }
}

Colour Grid::setTetriminoColour() {
    for (int i = 0; i < amountBlocks; i++) {
        Position blockPosition = (*currentBlocks)[i];
        blockPosition.x += upperLeft.x;
        blockPosition.y += upperLeft.y;
        gridMatrix[blockPosition.y][blockPosition.x]->setColour(*currentColour);
        (*currentBlocks)[i] = blockPosition;
    }
}

void Grid::addTetrimino(TetriminoType type){
    *currentType = type;
    upperLeft = Position{width/2, 0};
    setBoxDimension();
    currentTetrimino = new Tetrimino(type);
    currentBlocks = currentTetrimino->getBlocks();
    *currentColour = currentTetrimino->getColour();
    setTetriminoColour();
}

void Grid::setBoxDimension(){
    switch (*currentType) {
        case TetriminoType::O:
            boxDimension = 2;
            break;
        case TetriminoType::I:
            boxDimension = 4;
            break;
        default:
            boxDimension = 3;
            break;
    }
}

void Grid::moveTetrimino(Direction direction){
    if (direction == Direction::DOWN)
        makeFall();
    else
        moveToTheSides(direction);
}

void Grid::rotateTetrimino(){
    std::vector<Position> newBlocks = *currentBlocks;
    currentBlocks->clear();
    int count = 0, x = 0, y = 0;
    bool canRotate = true;
    Position position = upperLeft, position2 = Position{0, 0};

    while ((x < amountBlocks) && (count < amountBlocks) && canRotate) {
        position = upperLeft; y = 0;
        while ((y < amountBlocks) && (count < amountBlocks) && canRotate) {
            position.x+=x; position.y+=y;
            position2 = Position{upperLeft.x + boxDimension - y, upperLeft.y + x};   
            count += checkColoration(position, position2, &newBlocks);
            canRotate = count > -1;
            y++;
        }
        x++;
    }

    if (canRotate) 
        colorate();
    else
        *currentBlocks = newBlocks;
}

void Grid::moveTetrimino(Direction direction) {
    if ((direction == Direction::LEFT) || (direction == Direction::RIGHT))
        moveToTheSides(direction);
    else
        makeFall();
}

void Grid::moveToTheSides(Direction direction) {
    int movement = (direction == Direction::LEFT) ? -1 : 1;
    std::vector<Position> newBlocks = *currentBlocks;
    currentBlocks->clear();
    for (auto block : newBlocks) {
        bool isNotTetrimino = !isInTetrimino(block, &newBlocks);
        if (gridMatrix[block.y][block.x + movement]->getIsColoured() && isNotTetrimino) {
            *currentBlocks = newBlocks;
            return;
        }
        else
            currentBlocks->push_back(Position{block.x, block.y});
    }
    upperLeft.x += movement;
    for (auto block : (*currentBlocks)) {
        gridMatrix[block.y][block.x]->setdefaultColour();
        block.x += movement;
        gridMatrix[block.y][block.x]->setColour(*currentColour);
    }
}

bool Grid::checkCollision() {
    int movement = 1;
    for (auto block : (*currentBlocks)) {
        bool isNotTetrimino = !gridMatrix[block.y][block.x]->getIsColoured();
        if (gridMatrix[block.y + movement][block.x]->getIsColoured() && isNotTetrimino)
            return true;
    }
    return false;
}

void Grid::makeFall() {
    if (!checkCollision()) {
        int movement = 1;
        for (auto block : (*currentBlocks)) {
            block.y += movement;
            gridMatrix[block.y][block.x]->setColour(*currentColour);
        }
        upperLeft.y += movement;
    }
}

void Grid::colorate(){
    for (auto block : *currentBlocks) {
        gridMatrix[block.y + upperLeft.y][block.x + upperLeft.x]->setColour(*currentColour);
    }
}

int Grid::checkColoration(Position position, Position position2, std::vector<Position>* newBlocks) {
    bool isTetrimino = isInTetrimino(position, newBlocks);
    if (gridMatrix[position.y][position.x]->getIsColoured() && isTetrimino) {
        bool isNotInTetrimino = !isInTetrimino(position2, newBlocks);
        if (gridMatrix[position2.y][position2.x]->getIsColoured() && isNotInTetrimino)
            return -amountBlocks;
        gridMatrix[position.y][position.x]->setdefaultColour();
        currentBlocks->push_back(position2);
        return 1;
    }
    return 0;
}

bool Grid::isInTetrimino(Position position, std::vector<Position>* newBlocks) {
    std::vector<Position>* blocks = (newBlocks != nullptr) ? newBlocks : currentBlocks;
    for (auto block : *blocks) {
        if (block.x == position.x && block.y == position.y)
            return true;
    }
    return false;
}

Grid::~Grid(){
    for(int y=0; y<height; y++){
        for (int x=0; x<width; x++){
            delete gridMatrix[y][x];
        }
    }
    delete currentTetrimino;
}

Cell::Cell(int x, int y) {
    position.x = x;
    position.y = y;
    if (((x==0 || x==width-1) && y > tetriminoSpace-1) || y==height-1){
        isOutline = true;
        setColour(Colour::BLACK);
    }
}

void Cell::setColour(Colour newColour){
    this->colour = newColour;
    isColoured = true;
}

void Cell::setdefaultColour(){
    colour = Colour::WHITE;
    isColoured = false;
}

void Cell::setPosition(Position newPosition){
    position.x = newPosition.x;
    position.y = newPosition.y;
}

bool Cell::getIsColoured(){
    return isColoured;
}

Position Cell::getPosition(){
    return position;
}

Colour Cell::getColour(){
    return colour;
}
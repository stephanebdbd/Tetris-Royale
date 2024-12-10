#include "Grid.hpp"

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
    isColoured = (newColour != Colour::WHITE);
    isOutline = (newColour == Colour::BLACK);
    if (isOutline) isColoured = false;
}

void Cell::setdefaultColour(){
    colour = Colour::WHITE;
    isColoured = false;
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

void Cell::display(){
    std::cout << getColourChar(colour);  // Permet d'utiliser la fonction getColourChar de utils.hpp
}

Grid::Grid(){
    for(int y=0; y<height; y++){
        gridMatrix.push_back(std::vector<Cell*>());
        for (int x=0; x<width; x++){
            gridMatrix[y].push_back(new Cell(x, y));
        }
    }
}

void Grid::addTetrimino(Tetrimino* tetrimino){
        currentTetrimino = tetrimino;
        if (currentTetrimino != nullptr){
            currentType = currentTetrimino->getType();
            upperLeft = Position{width/2, 0};
            setBoxDimension();
            currentBlocks = currentTetrimino->getBlocks(currentType);
            currentColour = currentTetrimino->getColour();
            setTetriminoColour();
        }
}

void Grid::setBoxDimension(){
    switch (currentType) {
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

void Grid::setTetriminoColour() {
    for (int i = 0; i < amountBlocks; i++) {
        Position blockPosition = currentBlocks[i];
        blockPosition.x += upperLeft.x;
        blockPosition.y += upperLeft.y;
        gridMatrix[blockPosition.y][blockPosition.x]->setColour(currentColour);
        currentBlocks[i] = blockPosition;
    }
}

void Grid::moveTetrimino(Direction direction){
    Position movement = getMovement(direction);
    std::vector<Position> newBlocks;
    for (auto block : currentBlocks) {
        gridMatrix[block.y][block.x]->setdefaultColour();
        block.x += movement.x;
        block.y += movement.y;
        newBlocks.push_back(block);
    }
    upperLeft.x += movement.x;
    upperLeft.y += movement.y;
    currentBlocks = newBlocks;
    colorate();
}

Position Grid::getMovement(Direction direction) {
    Position mouvement = {0, 0};
    switch (direction) {
        case Direction::LEFT:
            mouvement.x = -1;
            break;
        case Direction::RIGHT:
            mouvement.x = 1;
            break;
        case Direction::DOWN:
            mouvement.y = 1;
            break;
        default:
            break;
    }
    return mouvement;
}

bool Grid::checkCollision(Direction direction) {
    Position movement = getMovement(direction);
    Position newPos = {0, 0};
    for (auto block : currentBlocks) {
        newPos = {block.x + movement.x, block.y + movement.y};
        if (getIsInGrid(newPos)) {
            bool isNotTetrimino = !isInTetrimino(newPos);
            if (gridMatrix[newPos.y][newPos.x]->getIsColoured() && isNotTetrimino){
                return true;
            }
        }
        else
            return true;
    }
    return false;
}

bool Grid::getIsInGrid(Position position){
    return ((0 < position.x) && (position.x < width-1) && (0 < position.y) && (position.y < height-1));
}

void Grid::rotateTetrimino(){
    std::vector<Position> oldBlocks = currentBlocks;
    currentBlocks.clear();
    int x = 0, y = 0;
    bool canRotate = true, isTetrimino = false;
    Position position = upperLeft, position2 = Position{0, 0};
    while ((y < boxDimension) && canRotate) {
        position.x = upperLeft.x-1; position.y = upperLeft.y + y;
        while ((x < boxDimension) && canRotate) {
            position.x++;
            isTetrimino = isInTetrimino(position, oldBlocks);
            if (getIsInGrid(position) && isTetrimino) {
                position2 = Position{upperLeft.x + boxDimension - 1 - y, upperLeft.y + x};   
                if (getIsInGrid(position2))
                    canRotate = checkColoration(position2, oldBlocks);
                else
                    canRotate = false;
            }
            x++;
        }
        y++; x = 0;
    }
    if (canRotate){
        for (auto block : oldBlocks)
            gridMatrix[block.y][block.x]->setdefaultColour();
        colorate();
    }
    else
        currentBlocks = oldBlocks;
}

void Grid::colorate(){
    for (auto block : currentBlocks)
        gridMatrix[block.y][block.x]->setColour(currentColour);
}

bool Grid::checkColoration(Position position, std::vector<Position> oldBlocks) {
    bool isNotInTetrimino = !isInTetrimino(position, oldBlocks);
    if (gridMatrix[position.y][position.x]->getIsColoured() && isNotInTetrimino)
        return false;
    currentBlocks.push_back(position);
    return true;
}

bool Grid::isInTetrimino(Position position, std::vector<Position> otherBlocks) {
    std::vector<Position> blocks = (otherBlocks.size() > 0) ? otherBlocks : currentBlocks;
    for (auto block : blocks) {
        if ((block.x == position.x) && (block.y == position.y))
            return true;
    }
    return false;
}

int Grid::checkLines(){
    LinesStatus linesStatus = {0, std::vector<int>()};
    int x = 1, y = height-2, count=2;
    while ((count > 1) && (y > tetriminoSpace-1)){
        count = 1;
        while ((x < width-1) && ((count==x) || (count==1))){
            if (gridMatrix[y][x]->getIsColoured()) count++;
            x++;
        }
        if (count == width-1) linesStatus.full++;
        else if (count > 1) linesStatus.coloured.push_back(y);
        x = 1; y--;
    }
    if (linesStatus.full > 0){
        int y=height-2;
        for (auto line : linesStatus.coloured){
            if (y != line){
                for (int x=1; x<width-1; x++){
                    Colour colour = gridMatrix[line][x]->getColour();
                    gridMatrix[y][x]->setColour(colour);
                }
            }
            y--;
        }
        for (int i = 0 ; i < linesStatus.full; i++){
            for (int x=1; x<width-1; x++)
                gridMatrix[y-i][x]->setdefaultColour();
        }
    }
    return linesStatus.full;
}

void Grid::exchangeColors(int tmp, int y){
    for (int x=1; x<width-1; x++){
        Colour tmpColour = gridMatrix[tmp][x]->getColour();
        Colour colour = gridMatrix[y][x]->getColour();
        gridMatrix[y][x]->setColour(tmpColour);
        gridMatrix[tmp][x]->setColour(colour);
    }
}

bool Grid::isTetriminoPlaced(){
    for (auto block : currentBlocks){
        if (block.y == height-2)
            return true;
        bool isTetrimino = isInTetrimino(Position{block.x, block.y+1});
        if ((gridMatrix[block.y+1][block.x]->getIsColoured()) && !isTetrimino)
            return true;
    }
    return false;
}

bool Grid::isGameOver(){
    if (isTetriminoPlaced()){
        for (auto block : currentBlocks){
            if (block.y < tetriminoSpace)
                return true;
        }
    }
    return false;
}

void Grid::display(){
    for (int y=0; y<height; y++){
        for (int x=0; x<width; x++){
            gridMatrix[y][x]->display();
        }
        std::cout << std::endl;
    }
}

Grid::~Grid(){
    for(int y=0; y<height; y++){
        for (int x=0; x<width; x++){
            delete gridMatrix[y][x];
        }
    }
    delete currentTetrimino;
}
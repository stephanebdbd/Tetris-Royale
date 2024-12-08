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
    isColoured = true;
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
    currentType = currentTetrimino->getType();
    upperLeft = Position{width/2, 0};
    setBoxDimension();
    currentBlocks = currentTetrimino->getBlocks();
    currentColour = currentTetrimino->getColour();
    setTetriminoColour();
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
    if (direction == Direction::DOWN)
        goDown();
    else
        moveToTheSides(direction);
}

void Grid::moveToTheSides(Direction direction) {
    Position movement = getMovement(direction);
    for (auto block : currentBlocks) {
        gridMatrix[block.y][block.x]->setdefaultColour();
        block.x += movement.x;
        gridMatrix[block.y][block.x]->setColour(currentColour);
    }
    upperLeft.x += movement.x;
}

void Grid::goDown() {
    int movement = 1;
    for (auto block : currentBlocks) {
        block.y += movement;
        gridMatrix[block.y][block.x]->setColour(currentColour);
    }
    upperLeft.y += movement;
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
    for (auto block : currentBlocks) {
        Position newPos = {block.x + movement.x, block.y + movement.y};
        bool isNotTetrimino = !gridMatrix[block.y][block.x]->getIsColoured();
        if (gridMatrix[newPos.y][newPos.x]->getIsColoured() && isNotTetrimino)
            return true;
    }
    return false;
}

void Grid::rotateTetrimino(){
    std::vector<Position> newBlocks = currentBlocks;
    currentBlocks.clear();
    int count = 0, x = 0, y = 0;
    bool canRotate = true;
    Position position = upperLeft, position2 = Position{0, 0};

    while ((x < amountBlocks) && (count < amountBlocks) && canRotate) {
        position = upperLeft; y = 0;
        while ((y < amountBlocks) && (count < amountBlocks) && canRotate) {
            position.x+=x; position.y+=y;
            position2 = Position{upperLeft.x + boxDimension - y, upperLeft.y + x};   
            count += checkColoration(position, position2, newBlocks);
            canRotate = count > -1;
            y++;
        }
        x++;
    }

    if (canRotate) 
        colorate();
    else
        currentBlocks = newBlocks;
}

void Grid::colorate(){
    for (auto block : currentBlocks) {
        gridMatrix[block.y + upperLeft.y][block.x + upperLeft.x]->setColour(currentColour);
    }
}

int Grid::checkColoration(Position position, Position position2, std::vector<Position> newBlocks) {
    bool isTetrimino = isInTetrimino(position, newBlocks);
    if (gridMatrix[position.y][position.x]->getIsColoured() && isTetrimino) {
        bool isNotInTetrimino = !isInTetrimino(position2, newBlocks);
        if (gridMatrix[position2.y][position2.x]->getIsColoured() && isNotInTetrimino)
            return -amountBlocks;
        gridMatrix[position.y][position.x]->setdefaultColour();
        currentBlocks.push_back(position2);
        return 1;
    }
    return 0;
}

bool Grid::isInTetrimino(Position position, std::vector<Position> newBlocks) {
    std::vector<Position> blocks = (newBlocks.size() > 0) ? newBlocks : currentBlocks;
    for (auto block : blocks) {
        if (block.x == position.x && block.y == position.y)
            return true;
    }
    return false;
}

void Grid::checkLines(int* lines){
    int readLines = height-1;
    while (readLines > 0){
        int tmp=-1, x=1;
        for (int y=height-1; y < boxDimension; y--){
            bool isLineUncolored = true;
            while (x < width-1 and isLineUncolored){
                isLineUncolored = !gridMatrix[y][x]->getIsColoured();
                x++;
                if (!isLineUncolored && tmp==-1)
                    tmp = y;
            }
            if (isLineUncolored && (tmp-y > 1)) {
                exchangeColors(tmp, y);
                tmp = -1; lines++;
            }
            else
                readLines--;
            x = 1;
        }
        if (readLines > 0)
            readLines = height-1;
    }
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
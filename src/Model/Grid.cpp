#include <iostream>
#include <Grid.hpp>


Grid::Grid(){
    for(int y=0; y<height; y++){
        for (int x=0; x<width; x++){
            gridMatrix[y][x] = new Cell(x, y);
        }
    }
}

void Grid::addTetrimino(TetriminoType type, Position upperLeft){
    CurrentTetrimino = new Tetrimino(type, upperLeft, this);
}

vector<vector<Cell*>>* Grid::getGrid(){
    return &gridMatrix;
}

void Grid::moveTetrimino(Direction direction){
    if (direction == Direction::DOWN)
        makeFall();
    else
        moveToTheSides(direction);
}

void Grid::rotateTetrimino(bool clockwise){
    CurrentTetrimino->rotate(clockwise);
}

Grid::~Grid(){
    for(int y=0; y<height; y++){
        for (int x=0; x<width; x++){
            delete gridMatrix[y][x];
        }
    }
    delete CurrentTetrimino;
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
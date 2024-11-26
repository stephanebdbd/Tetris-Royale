#include <iostream>
#include <Grid.hpp>


Grid::Grid(){
    for(int y=0; y<height; y++){
        for (int x=0; x<width; x++){
            gridMatrix[y][x] = new Cell(x, y);
        }
    }
}

void Grid::setPositions(Position position1, Position position2){
    int x1 = position1.x;
    int y1 = position1.y;
    int x2 = position2.x;
    int y2 = position2.y;
    Cell* tmp = gridMatrix[y1][x1];
    gridMatrix[y1][x1] = gridMatrix[y2][x2];
    gridMatrix[y2][x2] = tmp;
}

Grid::~Grid(){
    for(int y=0; y<height; y++){
        for (int x=0; x<width; x++){
            delete gridMatrix[y][x];
        }
    }
}

void Cell::setOutline(){
    if (((position.x == 0) || (position.x == width-1)) && (position.y < height-1))
            outline = Outline::SIDES;
    else {
        if (position.y == height-1)
            outline = Outline::BOTTOM;
        else if (position.x == 0)
                outline = Outline::ANGLE_DOWN_LEFT;
        else
            outline = Outline::ANGLE_DOWN_RIGHT;
    }
}

Cell::Cell(int x, int y){
    position.x = x;
    position.y = y;
    if (((x==0 || x==width-1) && y > tetriminoSpace-1) || y==height-1){
        isOutline = true;
        setOutline();
    }
}

void Cell::setColour(Colour colour){
    this->colour = colour;
    isColoured = true;
}

void Cell::setdefaultColour(){
    colour = Colour::BLACK;
    isColoured = false;
}

void Cell::setPosition(int x, int y){
    position.x = x;
    position.y = y;
}

bool Cell::getIsColoured(){
    return isColoured;
}

bool Cell::getIsOutline(){
    return isOutline;
}

int Cell::getPositionX(){
    return position.x;
}

int Cell::getPositionY(){
    return position.y;
}
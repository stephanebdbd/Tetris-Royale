#include <iostream>
#include <Grid.hpp>


Grid::Grid(){
    for(int y=0; y<height; y++){
        for (int x=0; x<width; x++){
            gridMatrix[y][x] = new Cell(x, y);
        }
    }
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

Cell::Cell(int x, int y) {
    position.x = x;
    position.y = y;
    if (((x==0 || x==width-1) && y > tetriminoSpace-1) || y==height-1){
        isOutline = true;
        setOutline();
    }
}

void Cell::setColour(Colour newColour){
    this->colour = newColour;
    isColoured = true;
}

void Cell::setdefaultColour(){
    colour = Colour::BLACK;
    isColoured = false;
}

void Cell::setPosition(Position newPosition){
    position.x = newPosition.x;
    position.y = newPosition.y;
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

Colour Cell::getColour(){
    return colour;
}
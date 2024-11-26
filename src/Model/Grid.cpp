#include <iostream>
#include <Grid.hpp>


Grid::Grid(){
    for(int y=0; y<height; y++){
        for (int x=0; x<width; x++){
            grid[y][x] = new Cell(x, y);
        }
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
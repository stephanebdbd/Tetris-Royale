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
    if (x==0 || x==width-1 || y > tetriminoSpace-1){ // A améliorer
        isOutline = true;
        setOutline();
    }
}

void Cell::setOutline(){ // A améliorer
    if ((position.x == 0) || (position.x == width-1))
            outline = Outline::SIDES;
    else
        switch (position.y){
            case height-1:
                outline = Outline::BOTTOM;
                break;
            default:
                break;
        }
}

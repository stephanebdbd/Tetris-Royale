#include "Controller.hpp"

Controller::Controller(Tetrimino *tetrimino, Grid *grid) : tetramino_{tetrimino}, grid_{grid} {}

void Controller::move(Direction direction){
    switch (direction) {
        case Direction::Left :
            if(canMove(Direction::Left)) tetramino_->move(-1, 0, grid_);
            break;
        case Direction::Right :
            if(canMove(Direction::Right)) tetramino_->move(1, 0, grid_);
            break;
        case Direction::Down :
        if(canMove(Direction::Down)) tetramino_->move(0, 1, grid_);
            break;
    }
}

void Controller::rotate(bool clockWise){
    tetramino_->rotate(clockWise);
}

bool Controller::canMove(Direction direction){}

void keybordInput(int keyInput){}

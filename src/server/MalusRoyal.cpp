#include "MalusRoyal.hpp"


MalusRoyal::MalusRoyal(TetraminoDisplacement& displacement): displacement(displacement){}

void MalusRoyal::reverseControl(){
    
    srand(time(0));
    int enter = rand() % 5;

    switch (enter) {
        case 0: displacement.setEnter(KEY_UP); break; 
        case 1: displacement.setEnter(KEY_DOWN); break; 
        case 2: displacement.setEnter(KEY_RIGHT); break; 
        case 3: displacement.setEnter(KEY_LEFT); break; 
        default: displacement.setEnter(' '); break;
    }

}

void MalusRoyal::increaseSpeed(){
    displacement.setSpeed(30);
}
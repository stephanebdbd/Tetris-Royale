#include "MalusRoyal.hpp"


MalusRoyal::MalusRoyal(std::shared_ptr<TetraminoDisplacement> displacement): displacement(displacement){}

void MalusRoyal::reverseControl(){
    
    srand(time(0));
    int enter = rand() % 5;

    switch (enter) {
        case 0: displacement->setEnter(KEY_UP); break; 
        case 1: displacement->setEnter(KEY_DOWN); break; 
        case 2: displacement->setEnter(KEY_RIGHT); break; 
        case 3: displacement->setEnter(KEY_LEFT); break; 
        case 4: displacement->setEnter(' '); break;
        default : break;
    }

}

void MalusRoyal::increaseSpeed(){
    displacement->setSpeed(5);
}
void MalusRoyal::blockControl(){
    displacement->setBlockCommand(true);
}
void MalusRoyal::turnOffLight(){
    displacement->setlightBlocked(true);
}   
void MalusRoyal::clear2x2Block(){
    displacement->random2x2MaskedBlock();
}
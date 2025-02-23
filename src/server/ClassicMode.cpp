#include "ClassicMode.hpp"
#include "Malus.hpp"
#include "Grid.hpp"

ClassicMode::ClassicMode(){}

void ClassicMode::feautureMode(Game& game, int linesCleared){

    int nbrMalus = getNbrMalus(linesCleared); // nombre du malus à envoyer

    if(nbrMalus > 0){
        useMalus(nbrMalus);
    }
}


void ClassicMode::useMalus(int nbrMalus){
    Malus malus(nbrMalus);
    int chosenPlayer = choosePlayer();

    if (!acceptChosenPlayer(chosenPlayer)){
        chosenPlayer = enterPlayer(); 
    }

    Grid& grid = playerGrids[chosenPlayer];
    malus.sendMalus(grid);
}


void ClassicMode::choosePlayer(){
    int C_Player = rand() % players.size();
    return players[C_Player].getId();
}

bool ClassicMode::acceptChosenPlayer(int chosenPlayer){
    cout<<"est ce que vous accepté le joueur "<<chosenPlayer<<" comme  malus (y/n)?"<<endl;
    char reponse;
    cin>>reponse;
    if(reponse == 'y'){
        return true;
    }
    return false;
}

int ClassicMode::enterPlayer(){
    int chosenPlayer;
    cout<<"entrer le joueur cible"<<endl;
    cin>>chosenPlayer;
    return chosenPlayer;
}

int ClassicMode::getNbrMalus(int nbrLineComplet) const{
    switch(nbrLineComplet){
        case 2 : return 1;
        case 3 : return 2;
        case 4 : return 4;
        default : return 0;  
    }
}
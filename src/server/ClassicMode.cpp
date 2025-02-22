#include "ClassicMode.hpp"
#include "Malus.hpp"

ClassicMode::ClassicMode() : GameMode("ClassicMode"){}

void ClassicMode::startGame(Game& game, int linesCleared){

    int nbrMalus = getNbrMalus(linesCleared); // nombre du malus à envoyer

    if(nbrMalus > 0){
        Malus malus(nbrMalus);
        int cible = chooseCible();

        if (!acceptPlayerCible(cible)){
            cible = enterCible(); 
        }

        Grid& grid = playerGrids[cible];
        malus.sendMalus(grid);
        
    }
}

void ClassicMode::chooseCible(){
    int C_Player = rand() % nbrPlayer;
    return Player[C_Player].getId();
}

bool ClassicMode::acceptPlayerCible(int cible){
    cout<<"est ce que vous accepté le joueur "<<cible<<" comme  malus (y/n)?"<<endl;
    char reponse;
    cin>>reponse;
    if(reponse == 'y'){
        return true;
    }
    return false;
}

int ClassicMode::enterCible(){
    int cible;
    cout<<"entrer le joueur cible"<<endl;
    cin>>cible;
    return cible;
}

int ClassicMode::getNbrMalus(int nbrLineComplet) const{
    switch(nbrLineComplet){
        case 2 : return 1;
        case 3 : return 2;
        case 4 : return 4;
        default : return 0;  
    }
}
#include "ClassicMode.hpp"
#include "Malus.hpp"

ClassicMode::ClassicMode(){}

void ClassicMode::feautureMode(Game& game){

    int linesCleared = game.getLinesCleared();
    int nbrMalus = getNbrMalus(linesCleared); // nombre du malus à envoyer

    if(nbrMalus > 0){
        Grid& grid = game.getGrid();
        malus.sendMalus(grid);
    }
}


int ClassicMode::getNbrMalus(int nbrLineComplet) const{
    switch(nbrLineComplet){
        case 2 : return 1;
        case 3 : return 2;
        case 4 : return 4;
        default : return 0;  
    }
}


/*void ClassicMode::useMalus(int nbrMalus){
    Malus malus(nbrMalus);
    int ClientId = choosePlayer();

    if (!acceptChosenPlayer(ClientId)){
        ClientId = enterPlayer(); 
    }

    Grid& grid = games[ClientId].getGrid();
    malus.sendMalus(grid);
}*/


/*void ClassicMode::choosePlayer(){
    int ClientId = rand() % games.size();
    return ClientId;
}*/

/*bool ClassicMode::acceptChosenPlayer(int chosenPlayer){
    mvprintw(0, 25, "est ce que vous acceptez d'envoyer malus au joueur %d (y/n)?", chosenPlayer);
    refresh();
    char respond = getch();
    
    move(0, 25);
    clrtoeol();
    refresh();

    return respond == 'y';
}*/

/*int ClassicMode::enterPlayer(){
    mvprintw(0, 25, "entrez le joueur ciblé");
    refresh();
    int chosenPlayer = getch();
    
    move(0, 25);
    clrtoeol();
    refresh();
    
    return chosenPlayer;
}*/


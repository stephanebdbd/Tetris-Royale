#include "malus.hpp"
#include <iostream>
#include <cstdlib>  // Pour rand() et srand()
#include <ctime> // pour time()
using namespace std ;
Malus::Malus(int nbrLine): nbrLine(nbrLine) { line = std::vector<char>(10, '#');}

void Malus:: createMalus(){
    std::srand(std::time(0));
    int nbrbloc = std::rand() % 10;
    line[nbrbloc] = ' ';
    for(int i = 0 ; i < nbrLine  ; i++){
        malus.push_back(line);
    }
}
std::vector<std::vector<char> > Malus:: getMalus(){
    return malus;
}
int Malus::getnbreLine(){
    return nbrLine;
}
int main() {
    Malus m(3); // Crée un objet `Malus` avec 3 lignes de malus
    m.createMalus(); // Génère 3 malus 
    std::vector<std::vector<char> > d = m.getMalus();
    for (const auto& ligne : d) { // Parcourt chaque ligne
        for (char c : ligne) {          // Parcourt chaque caractère de la ligne
            std::cout << c << " ";
        }
        std::cout << std::endl; // Passe à la ligne suivante
    }


    return 0;
}


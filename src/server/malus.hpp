#ifndef MALUS_HPP
#define MALUS_HPP
#include <vector>

class Malus {
    int nbrLine;
    std::vector<char> line;

    std::vector<std::vector<char> > malus;
    
    public:
        Malus(int nbrLine);
        void createMalus(); //creation de malus 
        int getnbreLine(); //renvoie de nopmbre de ligne qui represente le nombre de malus à créer 
        std::vector<std::vector<char> > sendMalus(); 
        std::vector<std::vector<char> > getMalus();



};



#endif

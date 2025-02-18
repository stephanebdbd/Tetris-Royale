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
        void drawMalus();



};



#endif

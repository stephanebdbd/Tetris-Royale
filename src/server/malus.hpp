#ifndef MALUS_HPP
#define MALUS_HPP

class Malus {
    int nbrLine;
    std::vector<char> line(10, '#');
    std::vector<std::array<std::array<char>>> malus;
    
    public:
        Malus(int nbrLine);
        void createMalus();
        std::vector<std::array<std::array<char>>> sendMalus();

}



#endif

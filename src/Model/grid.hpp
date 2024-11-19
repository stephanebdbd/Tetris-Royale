#include <iostream>
#include "tetrimino.hpp"


class Grid{
    private:
    static constexpr int width=10;
    static constexpr int height=20;

    public:
    Grid();
    bool addTetramino(Tetrimino& piece);
    void removeFullLines();
    bool isGameOver();
    void display();
};
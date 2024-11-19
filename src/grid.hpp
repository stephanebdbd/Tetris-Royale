#include <iostream>


class Grid{
    private:
    static constexpr int width=10;
    static constexpr int height=20;
    int cells[width][height];

    public:
    Grid();
    bool addTetramino(Tetrimino& piece);
    void removeFullLines();
    bool isGameOver();
    void display();

};
#include <iostream>
#include <string>
#include <./Model/Tetrimino.hpp>


class LeadBoard{
    int score;
    public:
    void UpdateScore(int score);

};

class PlayerBoard{
    bool gridState;
    Tetrimino activeTetrimino;
    Tetrimino nextTetrimino;
public:
    void UpdateBoard();
    void RenderBoard();
};
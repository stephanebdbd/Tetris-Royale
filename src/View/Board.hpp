#include <iostream>
#include <string>
#include <tetrimino.hpp>
class LeadBoard{
    private:
    int score;
    public:
    void UpdateScore(int score);

}

class PlayerBoard{
    private:
    bool gridState;
    Tetrimino activeTetrimino;
    Tetrimino nextTetrimino;
    public:
    UpdateBoard();
    RenderBoard();



}
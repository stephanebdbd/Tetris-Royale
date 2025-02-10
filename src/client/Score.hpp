#ifndef SCORE_HPP
#define SCORE_HPP

#include <ncurses.h>

class Score {
    int score;
    int x, y; // Position où afficher le score

    public:
        Score(int posX, int posY);
        void addScore(int linesCleared);
        void display() const;
        
        int getScore() const;
};

#endif

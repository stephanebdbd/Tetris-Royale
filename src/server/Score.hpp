#ifndef SCORE_HPP
#define SCORE_HPP

#include "../common/json.hpp"
#include <iostream>

class Score {
    int score, combo;
    int x, y; // Position où afficher le score

    public:
        Score(int posX, int posY);
        void addScore(int linesCleared);
        void display() const;
        int getScore() const;
        int getCombo() const;
        json scoreToJson() const;
};

#endif
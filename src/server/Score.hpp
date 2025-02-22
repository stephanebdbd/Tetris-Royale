#ifndef SCORE_HPP
#define SCORE_HPP

#include "../common/json.hpp"

class Score {
    int score, combo;
    int x, y; // Position oÃ¹ afficher le score

    public:
        Score(int posX, int posY);
        void addScore(int linesCleared);
        void display() const;
        int getScore() const;
        int getCombo() const;
        json scoreToJson() const;
};

#endif
#ifndef SCORE_HPP
#define SCORE_HPP

#include "../common/json.hpp"

using json = nlohmann::json;

class Score {
    int score;
    int x, y; // Position oÃ¹ afficher le score

    public:
        Score(int posX, int posY);
        void addScore(int linesCleared);
        void display() const;
        int getScore() const;
        json scoreToJson() const;
};

#endif
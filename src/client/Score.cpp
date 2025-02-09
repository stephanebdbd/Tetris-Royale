#include "Score.hpp"

Score::Score(int posX, int posY) : score(0), x(posX), y(posY) {}

void Score::addScore(int linesCleared) {
    int points[] = {0, 100, 300, 500, 800}; // Points attribués selon les lignes supprimées
    if (linesCleared >= 1 && linesCleared <= 4) {
        score += points[linesCleared];
    }
}

void Score::display() const {
    mvprintw(y, x, "Score: %d", score);
}

int Score::getScore() const {
    return score;
}

#include "Score.hpp"
#include "../common/jsonKeys.hpp"

#include <ncurses.h>

Score::Score(int posX, int posY) : score(0), combo(0), x(posX), y(posY) {}

void Score::addScore(int linesCleared) {
    int points[] = {0, 100, 300, 500, 800}; // Points attribués selon les lignes supprimées
    if (linesCleared >= 1 && linesCleared <= 4) {
        score += points[linesCleared];
        combo = linesCleared; // Augmente le combo du nombre de lignes détruites
    }
}

void Score::display() const {
    mvprintw(y, x, "Score: %d", score);
    mvprintw(y + 1, x, "Combo: %d", combo);
}

int Score::getScore() const {
    return score;
}

int Score::getCombo() const {
    return combo;
}

json Score::scoreToJson() const {
    json scoreJson;
    scoreJson[jsonKeys::SCORE] = score;
    scoreJson[jsonKeys::COMBO] = combo;
    return scoreJson;
}

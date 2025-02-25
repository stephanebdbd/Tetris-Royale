#ifndef CLIENTDISPLAY_HPP
#define CLIENTDISPLAY_HPP

#include "../common/json.hpp"


class ClientDisplay {

    public:
        void displayMenu(const json& data);
        void displayTetramino(const json& data);
        void displayGame(const json& data);
        void displayChat(const json& data);

        void drawGrid(const json& grid);
        void drawTetramino(const json& tetraPiece);
        void drawScore(const json& score);
};

#endif
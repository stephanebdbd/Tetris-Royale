#ifndef CLIENTDISPLAY_HPP
#define CLIENTDISPLAY_HPP

#include "../common/json.hpp"



class ClientDisplay {
    

    public:
        void displayMenu(const json& data, const std::string & inputBuffer);
        void displayTetramino(const json& data);
        void displayGame(const json& data);

        void drawGrid(const json& grid);
        void drawTetramino(const json& tetraPiece);
        void drawScore(const json& score);
        void drawMessage(const json& msg);
        void drawPlayerNumber(int playerNumber);

        void displayLargeText(const std::string& text, int startY, int startX, const std::string& colorSymbol);

};

#endif
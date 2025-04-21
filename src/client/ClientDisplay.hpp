#ifndef CLIENTDISPLAY_HPP
#define CLIENTDISPLAY_HPP

#include "../common/json.hpp"



class ClientDisplay {
    

    public:
        void displayMenu(const json& data, const std::string& inputBuffer);
        void displayGame(const json& data);

        void drawGrid(const json& grid);
        void drawTetramino(const json& tetraPiece);
        void drawScore(const json& score);
        void drawMessage(const json& msg);

        void displayLargeText(const std::string& text, int startY, int startX, const std::string& colorSymbol);

};

#endif
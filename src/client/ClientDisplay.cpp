#include "ClientDisplay.hpp"
#include "Color.hpp"

#include <ncurses.h>
#include <iostream>
#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"
#include <iostream>

void ClientDisplay::displayMenu(const json& data, const std::string& inputBuffer) {
    clear();

    std::string title = data[jsonKeys::TITLE];
    std::string input = data[jsonKeys::INPUT];
    json options = data[jsonKeys::OPTIONS];
    bool isText = data.contains(jsonKeys::TEXT);
    
    int y = 5;
    mvprintw(y++, 10, "%s", title.c_str());
    
    std::string line;
    for (auto& [key, value] : options.items()) {
        if (isText)
            line = value.get<std::string>();
        else 
            line = key + value.get<std::string>();
        mvprintw(y++, 10, "%s", line.c_str());
    }

    mvprintw(y++, 10, "%s%s", input.c_str(), inputBuffer.c_str());
    refresh(); 
}

void ClientDisplay::displayGame(const json& data) {
    //clear();
    for(int y = 0;y < 22; y++){
        move(y,0);
        clrtoeol();
        
    }

    drawGrid(data[jsonKeys::GRID]);

    drawTetramino(data[jsonKeys::TETRA_PIECE]);

    drawTetramino(data[jsonKeys::NEXT_PIECE]);

    drawScore(data[jsonKeys::SCORE]);
    
    drawMessage(data[jsonKeys::MESSAGE_CIBLE]);

    drawPlayerNumber(data[jsonKeys::PLAYER_NUMBER]);

    if(data[jsonKeys::MESSAGE_CIBLE][jsonKeys::GAME_OVER])
        displayLargeText("GAME OVER", 10, 1, "Z");

    refresh();
}

void ClientDisplay::drawGrid(const json& grid) {
    if(grid[jsonKeys::LIGHT_GRID]) return;
    int width = grid[jsonKeys::WIDTH]; 
    int height = grid[jsonKeys::HEIGHT]; 
    const json& cells = grid[jsonKeys::CELLS];

    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            // Voir si la cellule est occupée
            bool occupied = cells[y][x][jsonKeys::OCCUPIED];
            if (occupied) {
                int colorValue = cells[y][x][jsonKeys::COLOR];
                Color color = Color(static_cast<Type>(colorValue)); // Convertir la valeur en Type pour l'enum
                color.activate();
                mvaddch(y, x + 1, '#');
                color.deactivate();
            }
        }
    }

    for (int y = 0; y <= height; ++y) {
        mvaddch(y, 0, '|'); // Mur gauche
        mvaddch(y, width + 1, '|'); // Mur droit
    }
    for (int x = 0; x <= width + 1; ++x) {
        mvaddch(height, x, '-'); // Mur bas
    }
}

void ClientDisplay::drawTetramino(const json& tetraPiece) {
    if(tetraPiece[jsonKeys::LIGHT_TETRA]) return;
    // Récupération des informations
    int x = tetraPiece[jsonKeys::X];
    int y = tetraPiece[jsonKeys::Y];
    std::vector<std::vector<std::string>> shape = tetraPiece[jsonKeys::SHAPE];
    int shapeSymbol = tetraPiece[jsonKeys::SHAPE_SYMBOL];

    Color color = Color::fromShapeSymbol(std::string(1, shapeSymbol));

    color.activate();
    for (std::size_t row = 0; row < shape.size(); ++row) {
        for (std::size_t col = 0; col < shape[row].size(); ++col) {
            if (shape[row][col][0] != ' '){
                mvaddch(y + row, x + col, '#');
            }
        }
    }
    color.deactivate();
}

void ClientDisplay::drawScore(const json& score) {
    int scoreValue = score[jsonKeys::SCORE];
    mvprintw(1, 13, "Score: %d", scoreValue);
    int comboValue = score[jsonKeys::COMBO];
    mvprintw(2, 13, "Combo: %d", comboValue);
    

    }
void ClientDisplay::drawMessage(const json& msg){
    
    if (msg[jsonKeys::CLEAR]){
        for(int y = 22; y < 50; y++){
            move(y,0);
            clrtoeol();
        }
        
    }
    
    

    if(msg[jsonKeys::PROPOSITION_CIBLE]){
        int id = msg[jsonKeys::CIBLE_ID];
        mvprintw(22, 1, "Le joueur d'Id %d a été choisis comme joueur cible (Y/N): ", id);
    }
    
    else if(msg[jsonKeys::CHOICE_CIBLE])
        mvprintw(22, 1, "Entrez l'Id du joueur choisis: ");

    else if(msg[jsonKeys::CHOICE_MALUS_BONUS]){
        mvprintw(22, 1, "Saisiez votre choix : \n 1. Malus  \n 2. Bonus\n Choix: ");
    }
        

    else if(msg[jsonKeys::CHOICE_MALUS]){
        mvprintw(22, 1, "Saisiez le numéro de MALUS choisi : \n"
    "1. Inverser les commandes du joueur ciblé pour trois blocs.\n"
    "2. Bloquer les commandes du joueur ciblé pour un bloc.\n"
    "3. Accélérer la chute des pièces d’un adversaire.\n"
    "4. Supprimer une zone de 2X2 blocs chez un adversaire.\n"
    "5. Plonger l'écran du joueur ciblé dans le noir.\n"
    "Choix: ");
    }
    

    else if(msg[jsonKeys::CHOICE_BONUS])
        mvprintw(22, 1, "Saisiez le numéro de BONUS choisi :\n"
    "1. Ralentir la chute des pièces temporairement.\n"
    "2. Transformer les prochaines pièces en blocs 1X1.\n"
    "Choix: ");


    
    
}

void ClientDisplay::displayLargeText(const std::string& text, int startY, int startX, const std::string& colorSymbol) {
    Color color = Color::fromShapeSymbol(colorSymbol);

    color.activate();
    mvprintw(startY, startX, "%s", text.c_str());
    color.deactivate();
    
}
void ClientDisplay::drawPlayerNumber(int playerNumber) {
    mvprintw(3, 13, "Player Number: %d", playerNumber);
}
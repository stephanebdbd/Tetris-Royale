#include "ClientDisplay.hpp"
#include "Color.hpp"

#include <ncurses.h>
#include <iostream>
#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"

void ClientDisplay::displayMenu(const json& data) {
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

    mvprintw(y++, 10, "%s", input.c_str());
    refresh(); 
}

void ClientDisplay::displayGame(const json& data) {
    clear();

    drawGrid(data[jsonKeys::GRID]);

    drawTetramino(data[jsonKeys::TETRA_PIECE]);

    drawScore(data[jsonKeys::SCORE]);

    refresh();
}

void ClientDisplay::drawGrid(const json& grid) {
    int width = grid[jsonKeys::WIDTH]; 
    int height = grid[jsonKeys::HEIGHT]; 
    const json& cells = grid[jsonKeys::CELLS];

    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            //voir si la cellule est occupée
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

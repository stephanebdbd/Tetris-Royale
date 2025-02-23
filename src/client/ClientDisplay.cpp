#include "ClientDisplay.hpp"
#include "Color.hpp"

#include <ncurses.h>
#include "../common/json.hpp"

void ClientDisplay::displayMenu(const json& data) {
    clear();

    std::string title = data["title"];
    json options = data["options"];
    std::string input = data["input"];

    int y = 5;
    mvprintw(y++, 10, "%s", title.c_str());
    for (auto& [key, value] : options.items()) {
        std::string line = key + value.get<std::string>();
        mvprintw(y++, 10, "%s", line.c_str());
    }
    mvprintw(y++, 10, "%s", input.c_str());

    refresh(); 
}

void ClientDisplay::displayGame(const json& data) {
    clear();

    drawGrid(data["grid"]);

    drawTetramino(data["tetraPiece"]);

    drawScore(data["score"]);

    refresh();
}

void ClientDisplay::drawGrid(const json& grid) {
    int width = grid["width"]; 
    int height = grid["height"]; 
    const json& cells = grid["cells"];

    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            //voir si la cellule est occupée
            bool occupied = cells[y][x]["occupied"];
            if (occupied) {
                int colorValue = cells[y][x]["color"];
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
    int x = tetraPiece["x"];
    int y = tetraPiece["y"];
    std::vector<std::vector<std::string>> shape = tetraPiece["shape"];
    int shapeSymbol = tetraPiece["shapeSymbol"];

    Color color = Color::fromShapeSymbol(std::string(1, shapeSymbol));

    color.activate();
    for (size_t row = 0; row < shape.size(); ++row) {
        for (size_t col = 0; col < shape[row].size(); ++col) {
            if (shape[row][col][0] != ' '){
                mvaddch(y + row, x + col, '#');
            }
        }
    }
    color.deactivate();
}

void ClientDisplay::drawScore(const json& score) {
    int scoreValue = score["score"];
    mvprintw(1, 13, "Score: %d", scoreValue);
    int comboValue = score["combo"];
    mvprintw(2, 13, "Combo: %d", comboValue);
}
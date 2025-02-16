
#include "ClientDisplay.hpp"

void ClientDisplay::displayMenu(const json& data) {
    clear();

    std::string title = data["title"];
    json options = data["options"];
    std::string input = data["input"];

    int y = 5;
    mvprintw(y++, 10, "%s", title.c_str());
    for (auto& [key, value] : options.items()) {
        std::string line = key + ". " + value.get<std::string>();
        mvprintw(y++, 10, "%s", line.c_str());
    }
    mvprintw(y++, 10, "%s", input.c_str());

    refresh(); 
}

void ClientDisplay::displayTetramino(const json& data) {
    clear();
    json tetraPiece = data["tetraPiece"];

    // Récupération des données du Tétramino
    std::vector<std::vector<std::string>> shape = tetraPiece["shape"];
    int x = tetraPiece["x"];
    int y = tetraPiece["y"];

    // Affichage du Tétramino
    for (int row = 0; row < shape.size(); ++row) {
        for (int col = 0; col < shape[row].size(); ++col) {
            if (shape[row][col] != " ") {  // Si la case est occupée
                mvprintw(y + row, x + col, "%s", shape[row][col].c_str());
            }
        }
    }
    refresh();
}


void ClientDisplay::displayGame(const json& data) {

    drawGrid(data["grid"]);

    drawTetramino(data["tetraPiece"]);
}

void ClientDisplay::drawGrid(const json& grid) {
    int width = grid["width"]; 
    int height = grid["height"]; 
    const json& cells = grid["cells"];

    clear();
    int shift = 0;
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            int symbol = cells[y][x]["symbol"];
            mvaddch(shift + y, x + 1, static_cast<char>(symbol));
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
    std::vector<std::vector<std::string>> shape = tetraPiece["shape"];
    int x = tetraPiece["x"];
    int y = tetraPiece["y"];

    // Affichage du Tétramino
    for (int row = 0; row < shape.size(); ++row) {
        for (int col = 0; col < shape[row].size(); ++col) {
            if (shape[row][col] == "#") {  // Si c'est un bloc occupé
                mvprintw(y + row, x + col, "#");
            }
        }
    }

    refresh();
}

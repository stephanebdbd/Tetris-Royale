
#include "ClientDisplay.hpp"

using json = nlohmann::json;


void ClientDisplay::displayMenu(const json& data) {
    clear();

    std::string title = data["title"];
    json options = data["options"];
    std::string prompt = data["prompt"];

    int y = 5;
    mvprintw(y++, 10, "%s", title.c_str());
    for (auto& [key, value] : options.items()) {
        std::string line = key + ". " + value.get<std::string>();
        mvprintw(y++, 10, "%s", line.c_str());
    }
    mvprintw(y++, 10, "%s", prompt.c_str());

    refresh(); 
}

void ClientDisplay::displayTetramino(const nlohmann::json& data) {
    int x = data["x"];
    int y = data["y"];
    std::string symbol = data.contains("symbol") ? data["symbol"].get<std::string>() : "#";

    const auto& shape = data["shape"];

    for (size_t row = 0; row < shape.size(); ++row) {
        for (size_t col = 0; col < shape[row].size(); ++col) {
            if (shape[row][col] != " ") { // Si ce n'est pas un espace vide
                mvaddch(y + row, x + col, symbol[0]); // Affichage à la bonne position
            }
        }
    }

    refresh();
}


void ClientDisplay::displayGrid(const json& data) {
    clear();

    int width = data["grid"]["width"]; 
    int height = data["grid"]["height"]; 
    const json& cells = data["grid"]["cells"];

    drawGrid(width, height, cells);

    // Vérifier si un tétrimino est présent dans les données
    if (data.contains("tetramino")) {
        displayTetramino(data);
    }

    refresh();
}


void ClientDisplay::drawGrid(int width, int height, const json& cells) {
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


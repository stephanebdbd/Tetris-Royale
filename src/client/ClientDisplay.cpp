#include "ClientDisplay.hpp"
#include "Color.hpp"

#include <ncurses.h>
#include "../common/json.hpp"

void ClientDisplay::displayMenu(const json& data) {
    clear();

    // Vérifier que data est un objet JSON valide
    if (!data.is_object()) {
        mvprintw(5, 10, "Erreur : Données JSON invalides.");
        refresh();
        return;
    }

    // Récupérer le titre
    std::string title;
    if (data.contains("title") && data["title"].is_string()) {
        title = data["title"];
    } else {
        title = "Titre non disponible";
    }

    // Afficher le titre
    int y = 5;
    mvprintw(y++, 10, "%s", title.c_str());

    // Récupérer les options
    if (data.contains("options")) {
        json options = data["options"];

        // Vérifier si les options sont un tableau ou un objet JSON
        if (options.is_array()) {
            // Cas 1 : Les options sont un tableau de chaînes
            for (const auto& option : options) {
                if (option.is_string()) {
                    std::string line = option.get<std::string>();
                    mvprintw(y++, 10, "%s", line.c_str());
                }
            }
        } else if (options.is_object()) {
            // Cas 2 : Les options sont un objet JSON avec des paires clé-valeur
            for (const auto& [key, value] : options.items()) {
                std::string line = key;
                if (value.is_string()) {
                    line += value.get<std::string>();
                }
                mvprintw(y++, 10, "%s", line.c_str());
            }
        } else {
            mvprintw(y++, 10, "Erreur : Format d'options invalide.");
        }
    } else {
        mvprintw(y++, 10, "Aucune option disponible.");
    }

    // Récupérer et afficher l'invite de saisie
    std::string input;
    if (data.contains("input") && data["input"].is_string()) {
        input = data["input"];
    } else {
        input = "Entrée non disponible";
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

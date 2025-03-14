#include "ClientDisplay.hpp"
#include "Color.hpp"

#include <ncurses.h>
#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"

void ClientDisplay::displayMenu(const json& data) {
    clear();

    // Vérifier que data est un objet JSON valide
    if (!data.is_object()) {
        mvprintw(5, 10, "Erreur : Données JSON invalides.");
        refresh();
        return;
    }

    int y = 5; // Position verticale pour l'affichage

    // Espacement avant chaque section pour aérer le menu
    auto printSpace = [&y](int lines = 1) {
        for (int i = 0; i < lines; ++i) {
            mvprintw(y++, 10, " ");  // Ajouter une ligne vide pour espacement
        }
    };

    // Afficher le titre avec un peu de design
    std::string title = (data.contains("title") && data["title"].is_string()) ? data["title"] : "Titre non disponible";
    mvprintw(y++, 10, "*** %s ***", title.c_str());
    printSpace(1);  // Ajouter un espace après le titre

    // Afficher le message par défaut (si présent)
    if (data.contains("default") && data["default"].is_string()) {
        mvprintw(y++, 10, "[*] %s", data["default"].get<std::string>().c_str());
    }
    printSpace(1);  // Ajouter un espace après le message par défaut

    // Afficher les options (tableau ou objet JSON)
    if (data.contains("options")) {
        json options = data["options"];

        if (options.is_array()) {
            // Si les options sont un tableau, on les affiche ligne par ligne
            mvprintw(y++, 10, "Options disponibles :");
            printSpace(1);  // Ajouter un espace avant les options
            for (const auto& option : options) {
                if (option.is_string()) {
                    mvprintw(y++, 10, "%s", option.get<std::string>().c_str());
                }
            }
        } else if (options.is_object()) {
            // Si les options sont un objet, on les affiche sous forme "clé : valeur"
            mvprintw(y++, 10, "Options disponibles :");
            printSpace(1);  // Ajouter un espace avant les options
            for (const auto& [key, value] : options.items()) {
                std::string line = key + " " + value.get<std::string>();
                mvprintw(y++, 10, "%s", line.c_str());
            }
        } else {
            mvprintw(y++, 10, "Erreur : Format d'options invalide.");
        }
    } else {
        mvprintw(y++, 10, "Aucune option disponible.");
    }
    printSpace(1);  // Ajouter un espace après les options

    // Afficher le message d'aide, si présent
    if (data.contains("help") && data["help"].is_string()) {
        mvprintw(y++, 10, "[?] %s", data["help"].get<std::string>().c_str());
    }
    printSpace(1);  // Ajouter un espace après l'aide

    // Afficher l'invite pour la saisie utilisateur
    std::string input = (data.contains("input") && data["input"].is_string()) ? data["input"] : "Entrée non disponible";
    mvprintw(y++, 10, "> %s", input.c_str());
    printSpace(1);  // Ajouter un espace après l'invite de saisie

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
    int scoreValue = score[jsonKeys::SCORE];
    mvprintw(1, 13, "Score: %d", scoreValue);
    int comboValue = score[jsonKeys::COMBO];
    mvprintw(2, 13, "Combo: %d", comboValue);
}

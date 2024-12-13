#pragma once

#include "../include.hpp"


enum class Colour {
    BLACK,
    WHITE,
    ORANGE,
    LIGHT_BLUE,
    YELLOW,
    GREEN,
    RED,
    PURPLE,
    DARK_BLUE
};

struct Position{
    int x;
    int y;
};

struct nCurseColor {
    int code;
    int r;
    int g;
    int b;
};

enum class TetriminoType {
    I,
    O,
    T,
    S,
    Z,
    L,
    J,
};

enum class Direction {
    LEFT,
    RIGHT,
    DOWN,
};

const char ESC = '\x1b';

inline void setColors();

// Activer les couleurs
inline void enableColors() {
    if (!has_colors()) {
        endwin();
        std::cerr << "Le terminal ne supporte pas les couleurs" << std::endl;
        exit(1);
    }
    start_color();
    setColors();
}

// Définir des couleurs personnalisées avec init_color()
inline void setColors() {
    init_color(COLOR_RED, 1000, 0, 0);          // Rouge
    init_color(COLOR_YELLOW, 1000, 1000, 0);    // Jaune
    init_color(COLOR_GREEN, 0, 1000, 0);        // Vert
    init_color(COLOR_BLUE, 0, 0, 1000);         // Bleu foncé
    init_color(COLOR_CYAN, 0, 1000, 1000);      // Bleu clair
    init_color(COLOR_MAGENTA, 1000, 0, 1000);   // Mauve (magenta)
    init_color(COLOR_WHITE, 1000, 1000, 1000);  // Blanc
    init_color(COLOR_BLACK, 0, 0, 0);           // Noir
    init_color(9, 800, 500, 0);                 // Orange

    init_pair(1, COLOR_RED, COLOR_RED);         // Rouge sur rouge
    init_pair(2, COLOR_YELLOW, COLOR_YELLOW);   // Jaune sur jaune
    init_pair(3, COLOR_GREEN, COLOR_GREEN);     // Vert sur vert
    init_pair(4, COLOR_BLUE, COLOR_BLUE);       // Bleu foncé sur bleu foncé
    init_pair(5, COLOR_CYAN, COLOR_CYAN);       // Bleu clair sur bleu clair
    init_pair(6, COLOR_MAGENTA, COLOR_MAGENTA); // Mauve sur mauve
    init_pair(7, COLOR_WHITE, COLOR_WHITE);     // Blanc sur blanc
    init_pair(8, COLOR_BLACK, COLOR_BLACK);     // Noir sur noir
    init_pair(9, 9, 9);                        // Orange sur orange
}

inline void getColourChar(Colour colour) {
    switch (colour) {
        case Colour::BLACK:
            attron(COLOR_PAIR(8));
            printw("  ");
            attroff(COLOR_PAIR(8));
            break;
        case Colour::WHITE:
            attron(COLOR_PAIR(7));
            printw("  ");
            attroff(COLOR_PAIR(7));
            break;
        case Colour::ORANGE:
            attron(COLOR_PAIR(9));
            printw("  ");
            attroff(COLOR_PAIR(9));
            break;
        case Colour::LIGHT_BLUE:
            attron(COLOR_PAIR(5));
            printw("  ");
            attroff(COLOR_PAIR(5));
            break;
        case Colour::YELLOW:
            attron(COLOR_PAIR(2));
            printw("  ");
            attroff(COLOR_PAIR(2));
            break;
        case Colour::GREEN:
            attron(COLOR_PAIR(3));
            printw("  ");
            attroff(COLOR_PAIR(3));
            break;
        case Colour::RED:
            attron(COLOR_PAIR(1));
            printw("  ");
            attroff(COLOR_PAIR(1));
            break;
        case Colour::PURPLE:
            attron(COLOR_PAIR(6));
            printw("  ");
            attroff(COLOR_PAIR(6));
            break;
        case Colour::DARK_BLUE:
            attron(COLOR_PAIR(4));
            printw("  ");
            attroff(COLOR_PAIR(4));
            break;
        default:
            break;
    }
}
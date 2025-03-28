#include "Color.hpp"
#include <ncurses.h>
#include <unordered_map>
#include <iostream>

Color::Color(Type colorType) : type(colorType) {}

void Color::initialize() {
    start_color();
    use_default_colors(); 
    init_pair(1, COLOR_RED, -1);    // Z - Rouge
    init_pair(2, COLOR_YELLOW, -1); // O - Jaune
    init_pair(3, COLOR_GREEN, -1);  // S - Vert
    init_pair(4, COLOR_CYAN, -1);   // I - Cyan
    init_pair(5, COLOR_BLUE, -1);   // J - Bleu
    init_pair(6, COLOR_MAGENTA, -1);// T - Magenta
    init_pair(7, COLOR_WHITE, -1);  // L - Blanc
    init_pair(8, 244, -1); // Malus - Gris
}


Color Color::fromShapeSymbol(const std::string& shapeSymbol) {
    static std::unordered_map<std::string, Type> shapeColorMap = {
        {"I", Type::CYAN},
        {"L", Type::WHITE},
        {"J", Type::BLUE},
        {"T", Type::MAGENTA},
        {"O", Type::YELLOW},
        {"Z", Type::RED},
        {"S", Type::GREEN}
    };
    return Color(shapeColorMap[shapeSymbol]);
}


void Color::activate() const {
    initialize();
    attron(COLOR_PAIR(getId())); 
}

void Color::deactivate() const {
    attroff(COLOR_PAIR(getId()));
}

int Color::getId() const { 
    return static_cast<int>(type);
}
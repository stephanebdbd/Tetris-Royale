#pragma once

#include "../include.hpp"


enum class Colour {
    BLACK,
    WHITE,
    ORANGE,
    BLUE,
    YELLOW,
    GREEN,
    RED,
    PURPLE,
    BROWN
};

const char* getColour(Colour colour) {
    switch (colour) {
        case Colour::BLACK: return "⬛";
        case Colour::WHITE: return "⬜";
        case Colour::ORANGE: return "🟧";
        case Colour::BLUE: return "🟦";
        case Colour::YELLOW: return "🟨";
        case Colour::GREEN: return "🟩";
        case Colour::RED: return "🟥";
        case Colour::PURPLE: return "🟪";
        case Colour::BROWN: return "🟫";
        default: return "Unknown Colour";
    }
}

struct Position{
    int x;
    int y;
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
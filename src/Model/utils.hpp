#pragma once

#include "../include.hpp"

enum class Colour{
    BLACK = '⬛',    // Outline colour
    WHITE = '⬜',    // Default colour
    ORANGE = '🟧',
    BLUE = '🟦',
    YELLOW = '🟨',
    GREEN = '🟩',
    RED = '🟥',
    PURPLE = '🟪',
    BROWN = '🟫',
};

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
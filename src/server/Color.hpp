#ifndef COLOR_HPP
#define COLOR_HPP

#include <ncurses.h>
#include <string>

enum class Type {
    WHITE, RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA, NONE
};

class Color {
    Type type;

    public:
        Color(Type colorType);

        static void initialize(); 
        static Color fromShapeSymbol(const std::string& shapeSymbol);

        void activate() const;
        void deactivate() const;

        int getId() const;
};

#endif
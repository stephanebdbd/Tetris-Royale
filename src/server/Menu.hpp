#ifndef MENU_HPP
#define MENU_HPP

#include "../common/json.hpp"  // Inclusion de la bibliothèque JSON

class Menu {
    public:
        json getMainMenu0() const;  
        json getMainMenu1() const;  
        json getRegisterMenu() const;
};

#endif 
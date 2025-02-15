#ifndef MENU_HPP
#define MENU_HPP

#include "../common/json.hpp"  // Inclusion de la bibliothèque JSON

using json = nlohmann::json;

class Menu {
    public:
        json getMainMenu0();  
        json getMainMenu1();  
};

#endif

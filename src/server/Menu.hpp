#ifndef MENU_HPP
#define MENU_HPP

#include "../common/json.hpp"  // Inclusion de la bibliothèque JSON

class Menu {
    public:
        json getMainMenu0() const;  
        json getMainMenu1() const;  
        json getRegisterMenu1() const;
        json getRegisterMenuFailed() const;
        json getRegisterMenu2() const;
        json getLoginMenu1() const;
        json getLoginMenuFailed1() const;
        json getLoginMenu2() const;
        json getLoginMenuFailed2() const;
        json getJoinOrCreateGame() const;
        json getGameMode() const;
};

#endif 
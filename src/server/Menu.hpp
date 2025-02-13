#ifndef MENU_HPP
#define MENU_HPP

#include <string>

class Menu {
    int menuChoice;
    public:
        static std::string getMainMenu0();
        static std::string getMainMenu1();
        //getter du menuChoice
        int getMenuChoice();
};

#endif

#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include "../common/json.hpp"

struct GameState {
    json gridData;
    json scoreData;
    json nextPieceData;
    json currentPieceData;
    bool updated = false;  // drapeau pour indiquer que de nouvelles données sont arrivées

    json menu;
    bool isEnd = false;
    bool isGame = false;
    json message;
    json miniGrid;
    bool miniUpdate = false;

    std::vector<std::string> friendsLobby;
    std::map<std::string, std::vector<std::string>> pseudos;

    
};


#endif // GAMESTATE_HPP
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
        json getChatMenu() const;
        json getCreateChatRoomMenu() const;
        json getJoinChatRoomMenu(const std::vector<std::string>& chatRooms) const;
        json getListChatRoomsMenu(const std::vector<std::string>& chatRooms) const;
        json getRequestChatRoomMenu(const std::vector<std::string>& chatRooms) const;
        json getJoinOrCreateGame() const;
        json getGameMode() const;

        json getFriendMenu() const;
        json getAddFriendMenu() const;
        json getRemoveFriendMenu() const;
        json getFriendListMenu(const std::vector<std::string>& friends) const;
        json getRankingMenu(const std::vector<std::pair<std::string, int>>& ranking) const;
        json getGameOverMenu() const;        
        json getRequestsListMenu(const std::vector<std::string>& pendingRequests) const;
        json displayMessage(const std::string& message) const;

};

#endif 
#ifndef MENU_HPP
#define MENU_HPP

#include "../common/json.hpp"  // Inclusion de la bibliothèque JSON

class Menu {
    public:
        //welcome & main menu
        json getMainMenu0() const;  
        json getMainMenu1() const;
        //register & login  
        json getRegisterMenu1() const;
        json getRegisterMenuFailed() const;
        json getRegisterMenu2() const;
        json getLoginMenu1() const;
        json getLoginMenuFailed1() const;
        json getLoginMenu2() const;
        json getLoginMenuFailed2() const;
        //chat
        json getChatMenu() const;
        json getCreateJoinMenu() const;
        json getCreateChatRoomMenu() const;
        json getJoinChatRoomMenu() const;
        json getListe(const std::vector<std::string>& data, std::string title, const std::string& input) const;
        json getListeMembers(const std::vector<std::string>& data) const;
        json getManageChatRoomsMenu(const std::vector<std::string>& chatRooms) const;
        json getManageRoomMenu(bool isAdmin) const;
        json getAddDeleteMemberMenu(std::string title, std::string input) const;
        json getAddMembreMenu() const;
        json getRemoveMembreMenu() const;
        json getListeRequests(const std::vector<std::string>& data) const;
        json getQuitRoomMenu(bool isAdmin) const;

        //game
        json getJoinOrCreateGame() const;
        json getGameMode() const;

        //friends
        json getFriendMenu() const;
        json getAddFriendMenu() const;
        json getListFriendsMenu(const std::vector<int>& friends) const;
        json getRankingMenu(const std::vector<std::pair<std::string, int>>& ranking) const;
        json getGameOverMenu() const;        
        json getRequestsListMenu(const std::vector<std::string>& pendingRequests) const;
        json displayMessage(const std::string& message) const;

        json getGameModeMenu() const;
        json getLobbyMenu1() const;
        json getLobbyMenu2(int maxPlayers, const std::string& mode, int amountOfPlayers) const;
        json getGameRequestsListMenu(const std::vector<std::vector<std::string>>& pendingRequests) const;


};

#endif 
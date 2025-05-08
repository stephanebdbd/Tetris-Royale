#ifndef MENU_HPP
#define MENU_HPP

#include "../common/json.hpp"  // Inclusion de la bibliothèque JSON

class Menu {
    public:
        //welcome & main menu
        json getMainMenu0() const;  // welcome menu
        json getMainMenu1() const;  // Main menu
        //register & login  
        json getRegisterMenu1() const;
        json getRegisterMenuFailed() const;
        json getRegisterMenu2() const;
        json getLoginMenu1() const;
        json getLoginMenuFailed1() const;
        json getLoginMenu2() const;
        json getLoginMenuFailed2() const;
        //Teams
        json getTeamsMenu() const;
        json getCreateChatRoomMenu() const;
        json getJoinChatRoomMenu() const;
        json getInvitationsRoomsMenu(const std::vector<std::string>& invitations) const;
        json getListe(const std::vector<std::string>& data, std::string title, const std::string& input) const;
        json getListeMembers(const std::vector<std::string>& data) const;
        json getManageTeamsMenu(const std::vector<std::string>& chatRooms) const;
        json getManageTeamMenu(bool isAdmin, bool lastAdmin) const;
        json getAddMemberAdminMenu(std::string title, std::string input) const;
        json getAddMembreMenu() const;
        json getAddAdmin() const;
        json getListeRequests(const std::vector<std::string>& data) const;
        json getDeleteRoomConfirmationMenu(const std::string& roomName) const;
        json getQuitRoomConfirmationMenu(const std::string& roomName) const;
        //game
        json getHelpMenu(bool isRC, bool canEditMaxPlayer) const;
        json getJoinOrCreateGame() const;
        json getGameMode() const;
        json getGameModeMenu() const;
        json getLobbyMenu(int maxPlayers, const std::string& mode, int amountOfPlayers, int speed=0, int energy=0) const;
        json getGameRequestsListMenu(const std::vector<std::vector<std::string>>& pendingRequests) const;
        json getEndGameMenu(const std::string& message) const; 
        json getWinGameMenu() const;
        json getQuitGameMenu() const;

        //friends
        json getFriendMenu() const;
        json getAddFriendMenu() const;
        json getFriendListMenu(const std::vector<std::string>& friends) const;
        json getRequestsListMenu(const std::vector<std::string>& pendingRequests) const;

        //chat
        json getChooseContactMenu(const std::vector<std::string>& contacts, bool prive) const;
        
        //Ranking
        json getRankingMenu(const std::map<std::string, std::vector<std::string>>& ranking) const;
        json displayMessage(const std::string& message) const;
        
};

#endif 
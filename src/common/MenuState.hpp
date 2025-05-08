#ifndef MenuState_hpp
#define MenuState_hpp
#include <string>
#include <vector>
#include <sys/socket.h>
#include <ostream>
#include <iostream>
#include <map>


enum class MenuState {
    Welcome,
    Login,
        LoginPseudo,
        LoginPassword,
    Register,
        RegisterPseudo,
        RegisterPassword,
        RegisterConfirmPassword,
        
    Main,
        Notifications,
        Game,      
            JoinOrCreateGame,
                CreateGame,
                    ChooseGameMode,
                        Endless,
                        Duel,
                        Classic,
                        Royal_Competition,
                            Settings,
                            Observer,
                            Help,
                JoinGame,
                    GameRequestList,
                        Play,
                        GameOver,
                Pause,
        classement,
        Team,
            CreatTeamMenu,
            CreateTeam,
            JoinTeam,
            TeamsInvitation,
            ManageTeams,
                ManageTeam,
                    ListTeamMembres,
                    AddMembre,
                    AddAdmin,
                    RoomRequestList,
                    QuitRoom,
                    ConfirmDeleteRoom,
                    ConfirmQuitRoom,
            TeamChat,
                chat,
        Friends,
            AddFriend,
            FriendList,
            FriendRequestList,
            PrivateChat
};

class MenuStateManager {
public:
    MenuStateManager() = default;
    ~MenuStateManager() = default;

    // Sérialiser l'énumération MenuState en une chaîne de caractères
    std::string serialize(const MenuState& state);
    
    // Désérialiser la chaîne de caractères pour retrouver l'énumération MenuState
    MenuState deserialize(const std::string& data);
    
    // Envoyer l'état du menu au client
    void sendMenuStateToClient(int clientSocket, const MenuState& state, const std::string& message = "", const std::vector<std::string>& data = {}, const std::vector<std::pair<std::string, int>>& dataPair = {}, const std::map<std::string, std::vector<std::string>>& secondData = {});
    void sendTemporaryDisplay(int clientSocket, const std::string& message) ;

};

#endif
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
            PrivateChat,
        

};

class MenuStateManager {
public:
    MenuStateManager() = default;
    ~MenuStateManager() = default;

    // Serialize the MenuState enum to a string
    std::string serialize(const MenuState& state);
    // Deserialize the string back to MenuState enum
    MenuState deserialize(const std::string& data);
    // Send the MenuState to the client
    
    void sendMenuStateToClient(int clientSocket, const MenuState& state, const std::string& message = "", const std::vector<std::string>& data = {}, const std::vector<std::pair<std::string, int>>& dataPair = {}, const std::map<std::string, std::vector<std::string>>& secondData = {});
    void sendTemporaryDisplay(int clientSocket, const std::string& message) ;

};

#endif /* MenuState_hpp */
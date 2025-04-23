#ifndef MenuState_hpp
#define MenuState_hpp
#include <string>
#include <vector>
#include <sys/socket.h>
#include <ostream>
#include <iostream>


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
                            Help,
                JoinGame,
                    GameRequestList,
                        Play,
                        GameOver,
                Pause,
        classement,
        chat,
            CreateRoom,
            JoinRoom,
            RoomsInvitation,
            ManageRooms,
                ManageRoom,
                    ListRoomMembres,
                    AddMembre,
                    AddAdmin,
                    RoomRequestList,
                    QuitRoom,
                    ConfirmDeleteRoom,
                    ConfirmQuitRoom,
            PrivateChat,
        Friends,
            AddFriend,
            FriendList,
            FriendRequestList
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
    
    void sendMenuStateToClient(int clientSocket, const MenuState& state, const std::string& message = "", const std::vector<std::string>& data = {}, const std::vector<std::pair<std::string, int>>& dataPair = {}) ;
    void sendTemporaryDisplay(int clientSocket, const std::string& message) ;

};

#endif /* MenuState_hpp */
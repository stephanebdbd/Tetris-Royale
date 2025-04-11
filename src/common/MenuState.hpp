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
                JoinGame,
                    GameRequestList,
                        Play,
                        GameOver,
                Pause,
        classement,
        chat,
            CreateRoom,
            JoinRoom,
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
    
    void sendMenuStateToClient(int clientSocket, const MenuState& state, const std::string& message = "", const std::vector<std::string>& data = {});

};

#endif /* MenuState_hpp */
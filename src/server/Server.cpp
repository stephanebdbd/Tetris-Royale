#include "Server.hpp"
#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"
#include <ncurses.h>
#include <unistd.h>
#include <csignal>
#include <netinet/in.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <chrono>

Server::Server(int port)
    : port(port), 
      serverSocket(-1), 
      clientIdCounter(0), 
      database(std::make_shared<DataBase>()), 
      dataManager(database),
      chatRoomsManage(database), // Initialize chatRoom with the database instance
      chat(database) // Passez la base de données à ServerChat
      
{}



bool Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Erreur: Impossible de créer le socket du serveur." << std::endl;
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Erreur: Impossible de binder le socket." << std::endl;
        close(serverSocket);
        return false;
    }

    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Erreur: Impossible d'écouter sur le port." << std::endl;
        close(serverSocket);
        return false;
    }

    std::cout << "Serveur en attente de connexions sur le port " << port << std::endl;
    return true;
}

void Server::stop() {
    if (serverSocket != -1) {
        shutdown(serverSocket, SHUT_RDWR);
        close(serverSocket);
        serverSocket = -1;
    }
}

void Server::acceptClients() {
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientSocket < 0) {
        std::cerr << "Erreur: Échec de l'acceptation du client." << std::endl;
        return;
    }

    int clientId = clientIdCounter.fetch_add(1);  // Attribuer un ID unique et incrémenter le compteur
    std::cout << "Client #" << clientId << " connecté." << std::endl;
    
    clientStates[clientId] = MenuState::Welcome;

    clear();
    sendMenuToClient(clientSocket, menu.getMainMenu0()); 
    refresh();

    // Lancer un thread pour gérer ce client
    std::thread clientThread(&Server::handleClient, this, clientSocket, clientId);
    clientThread.detach();
}

void Server::sendMenuToClient(int clientSocket, const std::string& screen) {
    send(clientSocket, screen.c_str(), screen.size(), 0);
}

void Server::returnToMenu(int clientSocket, int clientId, MenuState state, const std::string& message, int sleepTime ) {
    if (!message.empty()) {
        sendMenuToClient(clientSocket, menu.displayMessage(message));
        sleep(sleepTime);
    }
    clientStates[clientId] = state;
    handleMenu(clientSocket, clientId, "", true); // Refresh the menu
}


void Server::handleClient(int clientSocket, int clientId) {
    char buffer[1024];
    auto lastRefreshTime = std::chrono::steady_clock::now();

    while (true) {
        // Handle input first
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, MSG_DONTWAIT); // Non-blocking read

        if (bytesReceived > 0) {
            // Process input
            try {
                json receivedData = json::parse(buffer);
                if (receivedData.contains(jsonKeys::MESSAGE)) {
                    // Handle chat messages
                    auto receiver = receiverOfMessages[clientId];
                    if(!chat.processClientChat(clientSocket, clientPseudo[clientId], receiver, receivedData, {pseudoTosocket[receiver]}))
                        clientStates[clientId] = MenuState::Friends;

                } else if (receivedData.contains(jsonKeys::ACTION)) {
                    std::string action = receivedData[jsonKeys::ACTION];
                    handleMenu(clientSocket, clientId, action, false); // Process action without refresh
                }
            } catch (json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            }
        } else if (bytesReceived == 0) {
            // Client disconnected
            cleanupClient(clientSocket, clientId);
            return;
        } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
            // Real error occurred
            std::cerr << "Receive error: " << strerror(errno) << std::endl;
            break;
        }

        // Handle refresh only when needed
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRefreshTime).count();
        
        if (elapsedTime >= 200) { // Reduced to 5 FPS (200ms) for better performance
            handleMenu(clientSocket, clientId, "", true); // Refresh only
            lastRefreshTime = currentTime;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Small sleep to prevent CPU overload
    }
}


void Server::cleanupClient(int clientSocket, int clientId) {
    close(clientSocket);

    // Supprimer le client de toutes les structures de données
    clientStates.erase(clientId);
    sockToPseudo.erase(clientSocket);
    pseudoTosocket.erase(clientPseudo[clientId]);
    clientPseudo.erase(clientId);
    runningChats.erase(clientSocket);
    roomToManage.erase(clientId);
    receiverOfMessages.erase(clientId);

    // Si le client était dans une salle de jeu, la supprimer
    if (clientStates[clientId] == MenuState::Play) {
        int roomId = clientGameRoomId[clientId];
        if ((roomId != -1)) {
            if (gameRooms[roomId]->getInProgress() || gameRooms[roomId]->getGameModeName() == GameModeName::Endless) {
                gameRooms[roomId]->endGame();
                deleteGameRoom(roomId);
            }
            else if (!gameRooms[roomId]->getHasStarted()) 
                gameRooms[roomId]->removePlayer(clientId);
        }
    }
    clientStates.erase(clientId); // Supprimer l'état des menus du client
    close(clientSocket); // Fermer la connexion
}


void Server::handleMenu(int clientSocket, int clientId, const std::string& action, bool refreshMenu) {
    switch (clientStates[clientId]) {
        case MenuState::Welcome:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getMainMenu0());
            else keyInputWelcomeMenu(clientSocket, clientId, action);
            break;
        case MenuState::RegisterPseudo:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getRegisterMenu1());
            else keyInputRegisterPseudoMenu(clientSocket, clientId, action);
            break;
        case MenuState::RegisterPassword:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getRegisterMenu2WithPseudo());
            else keyInputRegisterPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::LoginPseudo:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getLoginMenu1());
            else keyInputLoginPseudoMenu(clientSocket, clientId, action);
            break;
        case MenuState::LoginPassword:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getLoginMenu2WithPseudo());
            else keyInputLoginPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::Main:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getMainMenu1());
            else keyInputMainMenu(clientSocket, clientId, action);
            break;
        case MenuState::classement:
            //if (refreshMenu) sendMenuToClient(clientSocket, menu.getRankingMenu(dataManager.getRanking()));
            //else keyInputRankingMenu(clientSocket, clientId, action);
            break;
        case MenuState::Rooms:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getChatRoomsMenu());
            else keyInputChatRoomsMenu(clientSocket, clientId, action);
            break;
        case MenuState::CreateRoom:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getCreateChatRoomMenu());
            else keyInputCreateChatRoom(clientSocket, clientId, action);
            break;
        case MenuState::JoinRoom:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getJoinChatRoomMenu());
            else keyInputJoinChatRoom(clientSocket, clientId, action);
            break;
        case MenuState::RoomsRequest:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getInvitationsRoomsMenu(chatRoomsManage.getClientPending(clientPseudo[clientId])));
            else keyInputRoomsRequest(clientSocket, clientId, action);
            break;
        case MenuState::ChatRooms:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getChatRoomsMenu());
            else keyInputChatRoomsChooseMenu(clientSocket, clientId, action);
            break;
        case MenuState::ManageRooms:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getManageChatRoomsMenu(chatRoomsManage.getChatRoomsForUser(sockToPseudo[clientSocket])));
            else keyInputManageMyRooms(clientSocket, clientId, action);
            break;
        case MenuState::ManageRoom:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getManageRoomMenu(chatRoomsManage.isAdmin(sockToPseudo[clientSocket], roomToManage[clientId]), chatRoomsManage.getAdmins(roomToManage[clientId]).size() == 1));
            else keyInputManageRoom(clientSocket, clientId, action);
            break;
        case MenuState::ListRoomMembres:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getListeMembers(chatRoomsManage.getMembers(roomToManage[clientId])));
            else keyInputListMembres(clientSocket, clientId, action);
            break;
        case MenuState::AddMembre:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getAddMembreMenu());
            else keyInputAddMembre(clientSocket, clientId, action);
            break;
        case MenuState::AddAdmin:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getAddAdmin());
            else keyInputAddAdmin(clientSocket, clientId, action);
            break;
        case MenuState::RoomRequestList:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getListeRequests(chatRoomsManage.getClientPending(roomToManage[clientId])));
            else keyInputRequestList(clientSocket, clientId, action);
            break;
        case MenuState::ConfirmDeleteRoom:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getDeleteRoomConfirmationMenu(roomToManage[clientId]));
            else keyInputConfirmDeleteRoom(clientSocket, clientId, action);
            break;
        case MenuState::ConfirmQuitRoom:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getQuitRoomConfirmationMenu(roomToManage[clientId]));
            else keyInputQuitRoom(clientSocket, clientId, action);
            break;
        case MenuState::Friends:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getFriendMenu());
            else keyInputFriendsMenu(clientSocket, clientId, action);
            break;
        case MenuState::ChooseContact:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getChatWithRoomMenu(dataManager.getFriendList(clientPseudo[clientId])));
            else keyInputChatFriendsMenu(clientSocket, clientId, action);
            break;
        case MenuState::AddFriend:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getAddFriendMenu());
            else keyInputAddFriendMenu(clientSocket, clientId, action);
            break;
        case MenuState::FriendList:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getFriendListMenu(dataManager.getFriendList(clientPseudo[clientId])));
            else keyInputManageFriendlist(clientSocket, clientId, action);
            break;
        case MenuState::FriendRequestList:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getRequestsListMenu(dataManager.getRequestList(clientPseudo[clientId])));
            else keyInputManageFriendRequests(clientSocket, clientId, action);
            break;
        case MenuState::Game:
            clientStates[clientId] = MenuState::JoinOrCreateGame;
            [[fallthrough]];
        case MenuState::JoinOrCreateGame:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
            else keyInputJoinOrCreateGameMenu(clientSocket, clientId, action);
            break;
        case MenuState::JoinGame:
            //if (refreshMenu) sendMenuToClient(clientSocket, menu.getGameRequestsListMenu(dataManager.getListGameRequest(clientPseudo[clientId])));
            //else keyInputChoiceGameRoom(clientSocket, clientId, action);
            break;
        case MenuState::CreateGame:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getGameModeMenu());
            else keyInputChooseGameModeMenu(clientSocket, clientId, action);
            break;
        case MenuState::GameOver:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getEndGameMenu());
            else keyInputGameOverMenu(clientSocket, clientId, action);
            break;
        case MenuState::Settings:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getLobbyMenu2(getMaxPlayers(clientId), getMode(clientId), getAmountOfPlayers(clientId)));
            else keyInputLobbySettingsMenu(clientSocket, clientId, action);
            break;
        default:
            break;
    }
}


void Server::keyInputChooseGameModeMenu(int clientSocket, int clientId, const std::string& action){
    if(action == "1"){
        //Endless
        std::cout << "Client #" << clientId << " a sélectionné Endless." << std::endl;
        clientStates[clientId] = MenuState::Play;
        //clientStates[clientId] = MenuState::Settings;
        //sendMenuToClient(clientSocket, menu.getLobbyMenu());
        this->keyInputGameModeMenu(clientSocket, clientId);

    }
    else if(action == "2"){
        //Duel
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu2(2, "Duel", 1));
        this->keyInputGameModeMenu(clientSocket, clientId, GameModeName::Duel);
        
    }
    else if(action == "3"){
        //classic
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu1());
        this->keyInputGameModeMenu(clientSocket, clientId, GameModeName::Classic);
    }
    else if(action == "4"){
        //royal competition
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu1());
        this->keyInputGameModeMenu(clientSocket, clientId, GameModeName::Royal_Competition);
    }
    else if(action == "5"){
        clientStates[clientId] = MenuState::JoinOrCreateGame;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }
}


void Server::keyInputManageFriendRequests(int clientSocket, int clientId, const std::string& action) {
    std::string currentUser = clientPseudo[clientId];

    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
        return;
    }

    std::vector<std::string> requests = dataManager.getRequestList(currentUser);
    if (requests.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::FriendRequestList, "Vous n'avez aucune demande d'ami en attente.");
    }

    if (action == "accept.all" || action == "reject.all") {
        bool accept = (action == "accept.all");
        for (const std::string& friend_request : requests) {
            accept ? dataManager.acceptFriendRequest(currentUser, friend_request)
                   : dataManager.rejectFriendRequest(currentUser, friend_request);
        }

        returnToMenu(clientSocket, clientId, MenuState::Main,
            accept ? "Toutes les demandes d'amis ont été acceptées avec succès !" 
                   : "Toutes les demandes d'amis ont été rejetées avec succès !", 4);
        return;
    }

    std::string prefix_accept = "accept.";
    std::string prefix_reject = "reject.";
    bool isAccept = action.rfind(prefix_accept, 0) == 0;
    bool isReject = action.rfind(prefix_reject, 0) == 0;

    if (isAccept || isReject) {
        std::string friend_request = action.substr(isAccept ? prefix_accept.size() : prefix_reject.size());
        //if (!dataManager.hasSentRequest(friend_request, currentUser)) {
        //    returnToMenu(clientSocket, clientId, MenuState::Friends, "Aucune demande d'ami en attente de '" + friend_request + "'.");
        //    return;
        //}

        isAccept ? dataManager.acceptFriendRequest(currentUser, friend_request)
                 : dataManager.rejectFriendRequest(currentUser, friend_request);

        returnToMenu(clientSocket, clientId, MenuState::FriendRequestList,
            (isAccept ? "Demande d'ami acceptée avec " : "Demande d'ami rejetée pour '") + friend_request + "'.");
        return;
    }

}


void Server::keyInputManageFriendlist(int clientSocket, int clientId, const std::string& action) {
    std::string currentUser = clientPseudo[clientId];

    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
        return;
    }

    std::vector<std::string> friends = dataManager.getFriendList(currentUser);
    if (friends.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::FriendList, "Vous n'avez aucun ami dans votre liste.");
        return;
    }

    if (action == "del.all") {
        for (const std::string& friend_name : friends) {
            dataManager.deleteFriend(currentUser, friend_name);
        }

        returnToMenu(clientSocket, clientId, MenuState::Friends, "Tous vos amis ont été supprimés.");
        return;
    }

    std::string prefix_del = "del.";
    if (action.rfind(prefix_del, 0) == 0) {  // Vérifie si action commence par "del."
        std::string friend_name = action.substr(prefix_del.size());

        if (!dataManager.areFriends(currentUser, friend_name)) {
            returnToMenu(clientSocket, clientId, MenuState::FriendRequestList, "'" + friend_name + "' n'est pas dans votre liste d'amis.");
            return;
        }

        dataManager.deleteFriend(currentUser, friend_name);
        returnToMenu(clientSocket, clientId, MenuState::FriendRequestList, friend_name + " a été supprimé de votre liste d'amis.");
        return;
    }

    returnToMenu(clientSocket, clientId, MenuState::FriendRequestList, "Erreur : Format invalide. Utilisez 'del.pseudo' ou 'del.all'.");
}




void Server::keyInputFriendsMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        clientStates[clientId] = MenuState::AddFriend;
    }
    else if (action == "2") {
        clientStates[clientId] = MenuState::FriendList;
        
    }
    else if (action == "3") {
        clientStates[clientId] = MenuState::FriendRequestList;
    
    }
    else if (action == "4") {
        clientStates[clientId] = MenuState::ChooseContact;
    }
    else if (action == "5") {
        clientStates[clientId] = MenuState::Main;
    }else{
        sendMenuToClient(clientSocket, menu.getFriendMenu());
    }
}

/**
 * @brief Handles the key input for the "Add Friend" menu.
 * 
 * @note The section for checking if the user has already sent a friend request 
 *       is currently commented out. This needs to be implemented in the future.
 *       @todo Implement the check for whether the user has already sent a friend request.
 */
void Server::keyInputAddFriendMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
        return;
    }

    std::string currentUser;
    {
        std::lock_guard<std::mutex> lock(clientPseudoMutex);
        currentUser = clientPseudo[clientId];
    }
    std::string friend_request = trim(action);

    //Vérifier si l'utilisateur existe
    if (!dataManager.userExists(friend_request)) {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend, "L'utilisateur '" + friend_request + "' n'existe pas.");
        return;
    }
    // Vérifier si l'utilisateur est déjà dans la liste d'amis
    if (dataManager.areFriends(currentUser, friend_request)) {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend, "'" + friend_request + "' est déjà dans votre liste d'amis.");
        return;
    }
    // Vérifier si l'utilisateur a déjà envoyé une demande d'ami
    
    /*if (dataManager.hasSentRequest(currentUser, friend_request)) {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend, "Erreur : Vous avez déjà envoyé une demande d'ami à '" + friend_request + "'.");
        return;
    }*/


    // Vérifier si l'utilisateur essaie de s'ajouter lui-même
    if (friend_request == currentUser) {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend, "Vous ne pouvez pas vous ajouter vous-même.");
        return;
    }

    // Envoyer la demande d'ami
    dataManager.sendFriendRequest(currentUser, friend_request);
    returnToMenu(clientSocket, clientId, MenuState::Friends, "Demande d'ami'Demande d'ami envoyée à " + friend_request + ". Veuiller consulter la listes des amis pour voir si la demande a été acceptée.");
    
}


void Server::keyInputGameModeMenu(int clientSocket, int clientId, GameModeName gameMode) {
    //création de la gameRoom
    std::cout << "Création de la GameRoom pour le client #" << clientId << " avec le mode " << static_cast<int>(gameMode) << "." << std::endl;
    int gameRoomIndex = gameRooms.size();
    gameRooms.emplace_back(std::make_shared<GameRoom>(gameRoomIndex, clientId, gameMode));
    clientGameRoomId[clientId] = gameRoomIndex;
    
    if (gameRoomIndex != gameRoomIdCounter) {
        std::cerr << "Erreur: L'index de la GameRoom ne correspond pas à l'ID de la GameRoom." << std::endl;
        return;
    }
    
    auto gameRoom = gameRooms[gameRoomIndex];
    std::cout << "Dans keyInputGameModeMenu : " << gameRoom << std::endl;
    gameRoomIdCounter++;
    
    std::cout << "Démarrage du jeu pour le client #" << clientId << "." << std::endl;

    std::thread gameRoomThread(&GameRoom::startGame, gameRoom);
    std::thread loopgame(&Server::loopGame, this, clientSocket, clientId);
    std::thread inputThread(&Server::receiveInputFromClient, this, clientSocket, clientId);
    
    inputThread.join();
    loopgame.join();
    gameRoomThread.join();

    deleteGameRoom(gameRoom->getRoomId());
}

void Server::startGame(int clientSocket, int clientId){
    std::thread loopgame(&Server::loopGame, this, clientSocket, clientId);
    std::thread inputThread(&Server::receiveInputFromClient, this, clientSocket, clientId);
    
    loopgame.join();
    inputThread.join();
}


std::string Server::trim(const std::string& s) {
    const char* whitespace = " \t\n\r";
    std::size_t start = s.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    std::size_t end = s.find_last_not_of(whitespace);
    return s.substr(start, end - start + 1);
}

void Server::deleteGameRoom(int roomId) {
    if (roomId != gameRoomIdCounter - 1)
        shiftGameRooms(roomId);
    for (int idx=0; idx < clientIdCounter; idx++) {
        if (clientGameRoomId[idx] == roomId)
            clientGameRoomId[idx] = -1;
    }
    gameRooms.pop_back();
    gameRoomIdCounter--;
    std::cout << "GameRoom #" << roomId << " deleted." << std::endl;
}

void Server::shiftGameRooms(int index) {
    int size = gameRooms.size();
    for (int i = index; i < size - 1; ++i) {
        auto current = gameRooms[i];
        auto next = gameRooms[i + 1];
        current = next;
        current->setRoomId(i);
        for (auto& id : current->getPlayers())
            clientGameRoomId[id] = i;
    }
}

void Server::sendInputToGameRoom(int clientId, const std::string& action, std::shared_ptr<GameRoom> gameRoom) {
    gameRoom->input(clientId, action);
}

//recuperer les inputs du client
void Server::receiveInputFromClient(int clientSocket, int clientId) {
    auto gameRoom = gameRooms[clientGameRoomId[clientId]];
    char buffer[1024];

    std::cout << "Dans la réception des entrées du client " << gameRoom << std::endl;
    std::cout << "reception des entrées du client " << clientId << std::endl;
    while (true) {
        while ((clientStates[clientId] == MenuState::Play) && !gameRoom->getCanPlay()) continue;  // La partie commencera juste après la boucle
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); 
        if (bytesReceived > 0) {
            try {
                json receivedData = json::parse(buffer);
                std::string action = receivedData[jsonKeys::ACTION];

                std::cout << "Action reçue du client " << clientId << " : " << action << std::endl;

                if (clientStates[clientId] != MenuState::Play){
                    if (clientStates[clientId] == MenuState::Settings)
                        keyInputLobbySettingsMenu(clientSocket, clientId, action, gameRoom);
                    else
                        handleMenu(clientSocket, clientId, action);
                }
                else{
                    if ((!gameRoom->getGameIsOver(clientId)) && gameRoom->getInProgress())
                        sendInputToGameRoom(clientId, action, gameRoom);
                    }
                if ((clientStates[clientId] == MenuState::JoinOrCreateGame) || (clientStates[clientId] == MenuState::GameOver))
                    break;
                if ((gameRoom.get() == nullptr))
                    break;
                if (gameRoom->getCanGetGames() && gameRoom->getGameIsOver(clientId) && gameRoom->getHasStarted() && !gameRoom->getInProgress())
                    break;
                if (gameRoom->getRoomId() != clientGameRoomId[clientId])
                    break;
            }
            catch (json::parse_error& e) {
                std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
            }
        }
    }
    std::cout << "fini les entrées" << std::endl;
}

void Server::letPlayersPlay(const std::vector<int>& players) {
    for (int player : players)
        clientStates[player] = MenuState::Play;
}

void Server::loopGame(int clientSocket, int clientId) {
    std::cout << "Dans loopGame" << std::endl;

    if (clientGameRoomId.find(clientId) == clientGameRoomId.end()) {
        std::cerr << "Erreur: GameRoom introuvable pour le client #" << clientId << std::endl;
        return;
    }

    std::size_t gameRoomId = clientGameRoomId[clientId];

    if (gameRoomId >= gameRooms.size()) {
        std::cerr << "Erreur: GameRoom #" << gameRoomId << " introuvable." << std::endl;
        return;
    }

    auto gameRoom = gameRooms[gameRoomId];

    // attend que le GameRoom soit prêt à jouer
    while (!(gameRoom->getSettingsDone() || gameRoom->getOwnerQuit())) continue;


    if (!gameRoom->getOwnerQuit()) {

        while (!gameRoom->getCanGetGames()) continue;
        
        auto game = gameRoom->getGame(clientId);
        Score& score = game->getScore();

        if (clientId == gameRoom->getOwnerId() && (gameRoom->getGameModeName() != GameModeName::Endless))
            ownerStartsGame(gameRoom);
        else if (gameRoom->getGameModeName() == GameModeName::Endless)
            gameRoom->setToStartGame();

        while (!gameRoom->getCanPlay()) continue;

        std::cout << "Game #" << gameRoom->getRoomId() << " started." << std::endl;

        while (gameRoom->getInProgress()) { 

            if (gameRoom->getGameIsOver(clientId)) {
                if (!gameRoom->getInProgress()) break; 
            }

            if (gameRoom->getNeedToSendGame(clientId)) {
                sendGameToPlayer(clientSocket, game, score);
                gameRoom->setNeedToSendGame(false, clientId);
            }
        }

        /*if (gameRoom->getGameModeName() == GameModeName::Endless)
            dataManager->updateHighscore(clientPseudo[clientId], score.getScore());*/
    }

    else
        std::cout << "Owner #" << gameRoom->getOwnerId() << " a quitté la GameRoom #" << gameRoom->getRoomId() << std::endl;

    if ((gameRoom->getHasStarted() && (!gameRoom->getInProgress())) || (gameRoom->getOwnerQuit() && (gameRoom->getOwnerId() != clientId))) {
        clientStates[clientId] = MenuState::GameOver;
        sendMenuToClient(clientSocket, menu.getEndGameMenu());
    }

    std::cout << "Game #" << gameRoom->getRoomId() << " ended." << std::endl;
}

void Server::ownerStartsGame(std::shared_ptr<GameRoom> gameRoom) {
        letPlayersPlay(gameRoom->getPlayers());
        gameRoom->setToStartGame();
}


void Server::keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {  // Se connecter
        clientStates[clientId] = MenuState::LoginPseudo;
    }
    else if (action == "2") { // Créer un compte
        clientStates[clientId] = MenuState::RegisterPseudo;
    }
    else if (action == "3") {
        // Quitter
        std::cout << "Client #" << clientId << " déconnecté." << std::endl;
        // Fermer la connexion
        //sendMenuToClient(clientSocket, menu.getGoodbyeMessage());
        close(clientSocket);
        return;
    }
    else {
        sendMenuToClient(clientSocket, menu.getMainMenu0());
    }
}

void Server::keyInputRegisterPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Welcome);
        return;
    }
    if (dataManager.userNotExists(action)) { 
        // Si le pseudo n'existe pas, on stock en tmp
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::RegisterPassword;
    } 
    else {
        sendMenuToClient(clientSocket, menu.getRegisterMenu1());
    }
}

void Server::keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Welcome);
        return;
    }
    dataManager.registerUser(clientPseudo[clientId], action);
    sockToPseudo[clientSocket] = clientPseudo[clientId];
    pseudoTosocket[clientPseudo[clientId]] = clientSocket;
    clientStates[clientId] = MenuState::Main;
}


void Server::keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Welcome);
        return;
    }
    if (!dataManager.userNotExists(action)) { // Si le pseudo existe
        std::cout << "Pseudo existe" << std::endl;
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::LoginPassword;
    } 
    else {
        std::cout << "Pseudo n'existe pas" << std::endl;
        // si pseudo n'existe pas on annule et on retourne à l'étape 1 (dc dmd de pseudo)
        clientStates[clientId] = MenuState::RegisterPseudo;
        sendMenuToClient(clientSocket, menu.getLoginMenuFailed1());
    }
}


void Server::keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Welcome);
        return;
    }
    std::string currentUser = clientPseudo[clientId];

    if (dataManager.authenticateUser(currentUser, action)) { // Si le mot de passe est correct
        clientStates[clientId] = MenuState::Main;
        pseudoTosocket[currentUser] = clientSocket;
        sockToPseudo[clientSocket] = currentUser;
    } 
    else {
        // Si le mot de passe est incorrect, on retourne à l'étape 2 (dmd de mdp)
        sendMenuToClient(clientSocket, menu.getLoginMenuFailed2());
    }
}


void Server::keyInputMainMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") { // créer une gameRoom (choisir s'il veut créer sa partie ou rejoindre une partie)
        
        clientStates[clientId] = MenuState::Game;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }
    else if (action == "2") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
    }
    else if (action == "3") {
        // Classement
        returnToMenu(clientSocket, clientId, MenuState::classement);
    }
    else if (action == "4") {
        returnToMenu(clientSocket, clientId, MenuState::Rooms);

    }
    else if (action == "5") {
        // Paramètres
        returnToMenu(clientSocket, clientId, MenuState::Settings);
        
    }
    else if (action == "6") {
        // Retour à l'écran précédent
        returnToMenu(clientSocket, clientId, MenuState::Welcome);
    }
    else {
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    }
}

void Server::keyInputJoinOrCreateGameMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        // Créer une partie => lobby
        //clientStates[clientId] = MenuState::CreateGame;
        clientStates[clientId] = MenuState::CreateGame;
        sendMenuToClient(clientSocket, menu.getGameModeMenu());

        // Raccourci vers une game Endless car on doit implémenter le reste
        }
    else if (action == "2") {
        clientStates[clientId] = MenuState::JoinGame;
        std::string currentUser = clientPseudo[clientId];
        //std::vector<std::vector<std::string>> invitations = dataManager->getListGameRequest(currentUser);
        //sendMenuToClient(clientSocket, menu.getGameRequestsListMenu(invitations));
        // Rejoindre une partie
    }
    else if (action == "3") {
        clientStates[clientId] = MenuState::Main;
    }
    else {
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }
}


//Définir les actions possibles dans le chat
void Server::keyInputChatFriendsMenu(int clientSocket, int clientId, const std::string& action){
    if(action == "/back"){
        returnToMenu(clientSocket, clientId, MenuState::Main);
    }
    else if(dataManager.userNotExists(action)){
        returnToMenu(clientSocket, clientId, MenuState::ChooseContact, "User does not exist.");
    }else if( !dataManager.areFriends(sockToPseudo[clientSocket], action)){
        returnToMenu(clientSocket, clientId, MenuState::ChooseContact, "You are not friends with this user.");
    }else{
        receiverOfMessages[clientId] = action;                                  //stocker le pseudo du destinataire
        sendChatModeToClient(clientSocket);                                     //envoyer le mode de chat au client
        sleep(0.2); // Attendre un peu pour s'assurer que le client a reçu le mode de chat
        chat.sendOldMessages(clientSocket, sockToPseudo[clientSocket], action); //envoyer les anciens messages
        clientStates[clientId] = MenuState::PrivateChat;
    }
}


void Server::keyInputChatRoomsMenu(int clientSocket, int clientId, const std::string& action) {
    if(action == "1") {
        returnToMenu(clientSocket, clientId, MenuState::CreateRoom);
    }
    else if(action == "2") {
        returnToMenu(clientSocket, clientId, MenuState::JoinRoom);
    }
    else if(action == "3") {
        returnToMenu(clientSocket, clientId, MenuState::ChatRooms);
    }
    else if(action == "4") {
        returnToMenu(clientSocket, clientId, MenuState::RoomsRequest);
    }
    else if(action == "5") {
        returnToMenu(clientSocket, clientId, MenuState::ManageRooms);
    }
    else if(action == "6") {
        returnToMenu(clientSocket, clientId, MenuState::Main);
    }
    else{
        returnToMenu(clientSocket, clientId, MenuState::Rooms); 
    }
}


std::shared_ptr<std::vector<int>> Server::joinRoom(const std::string& roomName, int clientSocket) {
    std::lock_guard<std::mutex> lock(roomMutex);

    if (roomMap.find(roomName) == roomMap.end()) {
        roomMap[roomName] = std::make_shared<std::vector<int>>();
        std::cout << "Nouvelle room créée : " << roomName << std::endl;
    }

    auto roomSockets = roomMap[roomName];
    roomSockets->push_back(clientSocket);

    std::cout << "Client " << clientSocket << " a rejoint la room " << roomName << std::endl;

    return roomSockets;
}

void Server::keyInputChatRoomsChooseMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Rooms);
        return;
    }

    if (chatRoomsManage.checkroomExist(action)) { // Vérifie si la room existe
        if (chatRoomsManage.isClient(sockToPseudo[clientSocket], action)) {
            receiverOfMessages[clientId] = action;
            sendChatModeToClient(clientSocket);

            // 🟢 1. Ajouter le client dans la liste des sockets de la room
            auto roomSockets = joinRoom(action, clientSocket);

            // 🟢 2. Démarrer le chat de groupe (room)
            chatRoomsManage.processRoomChat(clientSocket, clientPseudo[clientId], action, roomSockets);
        } else {
            returnToMenu(clientSocket, clientId, MenuState::Rooms, "Vous n'êtes pas membre de cette room.");
        }
    } else {
        returnToMenu(clientSocket, clientId, MenuState::Rooms, "La room spécifiée n'existe pas.");
    }
}

void Server::keyInputCreateChatRoom(int clientSocket, int clientId, const std::string& action) {
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Rooms);
    }
    else if(chatRoomsManage.checkroomExist(action)){
        returnToMenu(clientSocket, clientId, MenuState::Rooms, "Room name already exists.");
    }
    else{
        //roomToManage[clientId] = action;
        chatRoomsManage.createRoom(action, sockToPseudo[clientSocket]);
        returnToMenu(clientSocket, clientId, MenuState::Rooms, "Room created successfully.");
    }

}

void Server::keyInputJoinChatRoom(int clientSocket, int clientId, const std::string& action) {
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Rooms);
    }
    else if(!chatRoomsManage.checkroomExist(action)){
        returnToMenu(clientSocket, clientId, MenuState::Rooms, "Room name does not exist.");
    }
    else if(chatRoomsManage.isClient(action, sockToPseudo[clientSocket])){
        returnToMenu(clientSocket, clientId, MenuState::Rooms, "You are already a member in this room");
    }
    else if(chatRoomsManage.isPendingReq(action, sockToPseudo[clientSocket])){
        returnToMenu(clientSocket, clientId, MenuState::Rooms, "You already sent a request to join this room");
    }
    else{
        chatRoomsManage.joinRoom(sockToPseudo[clientSocket], action);
        returnToMenu(clientSocket, clientId, MenuState::Rooms, "Request to join room sent succesfully.");
    }
}

void Server::keyInputRoomsRequest(int clientSocket, int clientId, const std::string& action) {
    std::vector<std::string> requests;
    std::string roomName = roomToManage[clientId];
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Rooms);
    }
    else if(action.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::Rooms);
    }
    else if(action.rfind("accept.", 0) == 0 || action.rfind("refuse.", 0) == 0){
        std::string option = action.substr(0, 7);
        std::string client = action.substr(7);
        //si la room est supprimé
        if(!chatRoomsManage.checkroomExist(roomName)){
            returnToMenu(clientSocket, clientId, MenuState::Rooms, "Room name does not exist.");
        }
        else{
            if(option == "accept."){
                chatRoomsManage.acceptClientRequest(client , roomName);
                returnToMenu(clientSocket, clientId, MenuState::Rooms, "Request to join room accepted.");
            }
            else{
                //chatRoomsManage.invitationResponse(sockToPseudo[clientSocket], false);
                returnToMenu(clientSocket, clientId, MenuState::Rooms, "Request to join room refused.");
            }
        }
    }else{
        returnToMenu(clientSocket, clientId, MenuState::Rooms);
    }
}


void Server::keyInputManageMyRooms(int clientSocket, int clientId, const std::string& action) {
    //std::vector<std::string> myRooms = chat->getMyRooms(sockToPseudo[clientSocket]);
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Rooms);
    }
    else if(action.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::ManageRooms);
    } 
    else if(!chatRoomsManage.checkroomExist(action)){ 
        returnToMenu(clientSocket, clientId, MenuState::Rooms, "Room name does not exist.");
    } 
    else {
        roomToManage[clientId] = action;
        clientStates[clientId] = MenuState::ManageRoom;
        sendMenuToClient(clientSocket, menu.getManageRoomMenu(chatRoomsManage.isAdmin(sockToPseudo[clientSocket],roomToManage[clientId]), chatRoomsManage.getAdmins(roomToManage[clientId]).size() == 1));
    }
}

void Server::keyInputManageRoom(int clientSocket, int clientId, const std::string& action) {
        std::string roomName = roomToManage[clientId];
        if(action == "1"){
            clientStates[clientId] = MenuState::ListRoomMembres;
            sendMenuToClient(clientSocket, menu.getListeMembers(chatRoomsManage.getMembers(roomName)));
            return;
        }
        bool isAdmin = chatRoomsManage.isAdmin(sockToPseudo[clientSocket], roomName);
        if(isAdmin) {
            if(action == "2") {
                clientStates[clientId] = MenuState::AddMembre;
                sendMenuToClient(clientSocket, menu.getAddMembreMenu());
            }
            else if(action == "3") {
                clientStates[clientId] = MenuState::AddAdmin;
                sendMenuToClient(clientSocket, menu.getAddAdmin());
            }
            else if(action == "4") {
                //les demandes d'ajout
                clientStates[clientId] = MenuState::RoomRequestList;
                sendMenuToClient(clientSocket, menu.getListeRequests(chatRoomsManage.getClientPending(roomName)));
            }
            //Supression de la room
            else if(action == "5") {
                if (chatRoomsManage.getMembers(roomName).size() == 1) {
                    returnToMenu(clientSocket, clientId, MenuState::ConfirmDeleteRoom);

                } else {
                    returnToMenu(clientSocket, clientId, MenuState::ConfirmQuitRoom );
                }
            }else if(action == "6") {
                //retour
                returnToMenu(clientSocket, clientId, MenuState::ManageRooms);
            }else{
                sendMenuToClient(clientSocket, menu.getManageRoomMenu(isAdmin, chatRoomsManage.getAdmins(roomName).size() == 1));
            }
        }
        else {
            if(action == "2") {
                returnToMenu(clientSocket, clientId, MenuState::ConfirmQuitRoom );
            }else if(action == "3") {
                //retour
                returnToMenu(clientSocket, clientId, MenuState::ManageRooms);
            }else{
                sendMenuToClient(clientSocket, menu.getManageRoomMenu(isAdmin, chatRoomsManage.getAdmins(roomName).size() == 1));
            }
        }
}
void Server::keyInputConfirmDeleteRoom(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if (action == "oui") {
        chatRoomsManage.deleteChatRoom(roomName);
        //chatRoomsManage.deleteRoomFile();
        //hatRooms.erase(roomName);
        returnToMenu(clientSocket, clientId, MenuState::ManageRooms, "Room '" + roomName + "' supprimée avec succès.");
    } else {
        returnToMenu(clientSocket, clientId, MenuState::ManageRooms, "Suppression de la room est refuseee.");
    }
}


void Server::keyInputListMembres(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }
    else if(action.rfind("del.", 0) == 0){
        std::string pseudo = action.substr(4);
        if (chatRoomsManage.isClient(pseudo,roomName)){
            chatRoomsManage.removeClient(pseudo,roomName);
            returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User removed successfully.");
        }else if (chatRoomsManage.removeClient(roomName, pseudo)){
            returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User removed successfully.");
        }
    }
    
    else {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }

}

void Server::keyInputAddMembre(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];

    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }
    else if (action.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::AddMembre);
    }
    else if (chatRoomsManage.isClient(action, roomName)) { 
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User is already in the room.");
    }
    else if (dataManager.userNotExists(action)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User does not exist.");
    }
    else if (chatRoomsManage.addClient(action, roomName)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "Request sent successfully.");
    }
    else {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "Failed to add user.");
    }
}


void Server::keyInputAddAdmin(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }
    else if(action.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::AddAdmin);
    }
    else if(chatRoomsManage.isAdmin(action, roomName)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User is already an admin.");
    }
    else if(!chatRoomsManage.isClient(action, roomName)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User is not in the room.");
    }
    else {
        chatRoomsManage.addAdmin(action, roomName);
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User is now an admin.");
    }
}

void Server::keyInputRequestList(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }
    else if(action.rfind("accept.", 0) == 0){
        std::string pseudo = action.substr(7);
        if (chatRoomsManage.isPendingReq(pseudo, roomName )){
            chatRoomsManage.acceptClientRequest(pseudo, roomName);
            returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User added successfully.");
        }
        else if(chatRoomsManage.isClient(pseudo, roomName)){
            returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User is already in the room.");
        }
    }
    else if(action.rfind("reject.", 0) == 0){
        std::string pseudo = action.substr(7);
        if (chatRoomsManage.isPendingReq(pseudo, roomName)){
            chatRoomsManage.refuseClientRequest(pseudo, roomName);
            returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User removed successfully.");
        }
        else if(chatRoomsManage.isClient(pseudo, roomName)){
            returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User is already in the room.");
        }
    }
    else {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }
}

void Server::keyInputQuitRoom(int clientSocket, int clientId, const std::string& action){
    std::string roomName = roomToManage[clientId];
    std::string userPseudo = sockToPseudo[clientSocket];

    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
        return;
    }

    /*if (chatRooms.find(roomName) == chatRooms.end()) {
        returnToMenu(clientSocket, clientId, MenuState::ManageRooms, "Room does not exist.");
        return;
    }*/
    bool isAdmin = chatRoomsManage.isAdmin(userPseudo, roomName);
    if (action == "oui"){
        if (isAdmin) {
            if (chatRoomsManage.getAdmins(roomName).size() > 1) {
                chatRoomsManage.removeAdmin(userPseudo, roomName);
                returnToMenu(clientSocket, clientId, MenuState::ManageRooms, "You have left the room.");
            } else {
                chatRoomsManage.deleteChatRoom(roomName);
                returnToMenu(clientSocket, clientId, MenuState::ManageRooms, "Room deleted successfully.");
            }
        } else {
            chatRoomsManage.removeClient(userPseudo, roomName);
            returnToMenu(clientSocket, clientId, MenuState::ManageRooms, "You have left the room.");
        }        
    }else{
        //retour au menu de gestion de la room (si on a pas quitté/ supprimé la room)
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }

}


//Définir les actions possibles dans le classement

void Server::keyInputRankingMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "/back") { // TODO: faudra qu'on le voit dans le menu
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    }
}

void Server::keyInputGameOverMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        clientStates[clientId] = MenuState::JoinOrCreateGame;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }
    else if (action == "2") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    }
}


void Server::sendGameToPlayer(int clientSocket, std::shared_ptr<Game> game, Score& score) {
    json message;

    message[jsonKeys::SCORE] = score.scoreToJson();
    message[jsonKeys::GRID] = game->getGrid().gridToJson();
    message[jsonKeys::TETRA_PIECE] = game->getCurrentPiece().tetraminoToJson(); // Ajout du tétrimino dans le même message

    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0); // Un seul envoi
}

void Server::sendChatModeToClient(int clientSocket) {
    json message;
    message[jsonKeys::MODE] = "chat";
    message["pseudo"] = sockToPseudo[clientSocket];
    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0);
}


void Server::keyInputSendGameRequestMenu(int /*clientSocket*/, int /*clientId*/, std::string /*receiver*/, std::string /*status*/) {    
    /*
    std::cout << "Sending game request to: " << receiver << " with status: " << status << std::endl;

    std::string game_request = receiver;
    std::string currentUser = clientPseudo[clientId];

    if(game_request == currentUser){
        sendMenuToClient(clientSocket, menu.displayMessage("Erreur : Vous ne pouvez pas vous envoyer une invitation de jeu."));
        sleep(3);
        return;
    }
    if(!dataManager->areFriends(currentUser,game_request)){
        sendMenuToClient(clientSocket, menu.displayMessage("Erreur : Vous n'êtes pas ami avec "+game_request+"."));
        sleep(3);
        return;
    }
    int gameRoomId = clientGameRoomId[clientId];
    dataManager->sendInvitationToFriend(currentUser, game_request,status, gameRoomId);
    sendMenuToClient(clientSocket, menu.displayMessage("Request Sent To "+game_request+"."));
    sleep(3);*/

}



void Server::keyInputLobbySettingsMenu(int clientSocket, int clientId, const std::string& action, std::shared_ptr<GameRoom> gameRoom) {

    std::cout << "Received action: " << action << " from client #" << clientId << std::endl;

    // Vérifier si l'action commence par \invite
    if (action.find("/invite") == 0) {
        // Extraire le statut et le receiver de l'action
        std::size_t firstBackSlash = action.find('/', std::string("/invite").size());  // Trouver le premier \ après \invite
        if (firstBackSlash == std::string::npos) {
            // Si le format est incorrect, retourner sans faire rien
            return;
        }

        std::size_t secondBackSlash = action.find('/', firstBackSlash + 1);  // Trouver le deuxième \ après le statut
        if (secondBackSlash == std::string::npos) {
            // Si le format est incorrect, retourner sans faire rien
            return;
        }

        // Extraire le status (par exemple: observer, player)
        std::string status = action.substr(firstBackSlash + 1, secondBackSlash - firstBackSlash - 1); 
        
        // Extraire le receiver (nom du joueur)
        std::string receiver = action.substr(secondBackSlash + 1);  

        // Appeler la méthode pour gérer l'envoi de la demande de jeu
        keyInputSendGameRequestMenu(clientSocket, clientId, receiver, status);        
    }
    
    else if (action.find("/back") == 0){
        auto gameRoom = gameRooms[clientGameRoomId[clientId]];
        if (gameRoom->getOwnerId() == clientId)
        gameRoom->setOwnerQuit();
        else
        gameRoom->removePlayer(clientId);
        clientStates[clientId] = MenuState::JoinOrCreateGame;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
        return;
    }
    
    else if (gameRoom != nullptr)
        gameRoom->input(clientId, action);
    
    if ((gameRoom != nullptr) && (!gameRoom->getHasStarted()))   //refresh le Lobby
        sendMenuToClient(clientSocket, menu.getLobbyMenu2(getMaxPlayers(clientId), getMode(clientId), getAmountOfPlayers(clientId)));
}

void Server::keyInputChoiceGameRoom(int clientSocket, int clientId, const std::string& action){
    if (action.find("accept.") == 0){
        std::size_t pos = action.find(".");
        std::string number = action.substr(pos+1, action.size());
        int roomNumber = std::stoi(number);
        clientGameRoomId[clientId] = roomNumber;
        auto gameRoom = gameRooms[roomNumber];
        gameRoom->addPlayer(clientId);
        clientStates[clientId] = MenuState::Settings;
        
        for (const auto& cId : gameRooms[roomNumber]->getPlayers())
            sendMenuToClient(pseudoTosocket[clientPseudo[cId]], menu.getLobbyMenu2(getMaxPlayers(cId), getMode(cId), getAmountOfPlayers(cId)));

        startGame(clientSocket, clientId);


    }
    else if(action.find("/back") == 0){
        clientStates[clientId] = MenuState::JoinOrCreateGame;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }

}

std::string Server::getMode(int clientId){
    int gameRoomId = clientGameRoomId[clientId];
    GameModeName mode = gameRooms[gameRoomId]->getGameModeName();
    std::string modestr;

    switch(mode){
        case GameModeName::Endless:
            modestr = "Endless";
            break;
        case GameModeName::Duel:
            modestr = "Duel";
            break;
        case GameModeName::Classic:
            modestr = "Classic";
            break;
        case GameModeName::Royal_Competition:
            modestr = "Royal Competition";
            break;
        default:
            modestr = "";
            break;

    }
    return modestr;
}

int Server::getMaxPlayers(int clientId){
    int gameRoomId = clientGameRoomId[clientId];
    return gameRooms[gameRoomId]->getMaxPlayers();
}

int Server::getAmountOfPlayers(int clientId){
    int gameRoomId = clientGameRoomId[clientId];
    return gameRooms[gameRoomId]->getAmountOfPlayers();
}




int main() {
    // le client ne doit pas l'igniorer faudra sans doute faire un handler pour le SIGPIPE ? 
    signal(SIGPIPE, SIG_IGN);  // le client arrivait à crasher le serveur en fermant la connexion
    
    try {
        std::ofstream serverLog("server.log"); // Créer un fichier de log
        // Rediriger std::cout et std::cerr vers le fichier log
        std::cout.rdbuf(serverLog.rdbuf());
        std::cerr.rdbuf(serverLog.rdbuf());


        Server server(12345);
        if (!server.start()) {
            std::cerr << "Erreur: Impossible de démarrer le serveur." << std::endl;
            return 1;
        }

        while (true) {
            server.acceptClients();
        } 

        server.stop();
    } 
    catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }
    return 0;
}


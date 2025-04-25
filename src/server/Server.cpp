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
    :   port(port), 
        serverSocket(-1), 
        clientIdCounter(0), 
        database(std::make_shared<DataBase>()), 
        userManager(database),
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
    clientIdToSocket[clientId] = clientSocket; 
    clientStates[clientId] = MenuState::Welcome;

    clear();
    sendMenuToClient(clientSocket, menu.getMainMenu0()); 
    refresh();

    // Lancer un thread pour gérer ce client
    std::thread clientThread(&Server::handleClient, this, clientSocket, clientId);
    clientThread.detach();
}

void Server::handleClient(int clientSocket, int clientId) {
    char buffer[1024];
    constexpr int TARGET_FPS = 7;
    constexpr std::chrono::milliseconds FRAME_TIME(1000 / TARGET_FPS);
    
    auto lastRefreshTime = std::chrono::steady_clock::now();
    auto nextRefreshTime = lastRefreshTime + FRAME_TIME;

    while (true) {
        // Vérification des données entrantes en mode non-bloquant
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(clientSocket, &readSet);
        
        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 5000; // 5ms timeout pour vérifier régulièrement
        
        int selectResult = select(clientSocket + 1, &readSet, nullptr, nullptr, &timeout);
        
        if (selectResult < 0) {
            std::cerr << "Erreur select() pour client #" << clientId << std::endl;
            break;
        }
        
        if (selectResult > 0 && FD_ISSET(clientSocket, &readSet)) {
            // Données disponibles à lire
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            
            if (bytesReceived <= 0) {
                std::cout << "Client #" << clientId << " déconnecté." << std::endl;
                if (clientStates[clientId] == MenuState::Play) {
                    std::lock_guard<std::mutex> lock(gameRoomsMutex);
                    int roomId = clientGameRoomId[clientId];
                    if ((roomId != -1)) {
                        if (gameRooms[roomId]->getInProgress() || gameRooms[roomId]->getGameModeName() == GameModeName::Endless)
                            gameRooms[roomId]->endGame();
                    }
                    disconnectPlayer(clientId);
                }
                return;
            }
            
            try {
                json receivedData = json::parse(buffer);
                if (receivedData.contains(jsonKeys::MESSAGE)) {
                    handleChat(clientSocket, clientId, receivedData);
                    continue;
                }
                if (!receivedData.contains(jsonKeys::ACTION) || !receivedData[jsonKeys::ACTION].is_string()) {
                    std::cerr << "Erreur: 'action' manquant ou invalide dans le JSON reçu." << std::endl;
                    continue;
                }
                
                std::string action = receivedData[jsonKeys::ACTION];
                handleMenu(clientSocket, clientId, action);
            } catch (json::parse_error& e) {
                std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
            }
        }
        
        // Gestion du rafraîchissement à 12 FPS
        auto currentTime = std::chrono::steady_clock::now();
        if (currentTime >= nextRefreshTime) {
            handleMenu(clientSocket, clientId, "", true); // Refresh only
            lastRefreshTime = currentTime;
            nextRefreshTime = lastRefreshTime + FRAME_TIME;
            
            // Ajustement dynamique si on prend du retard
            if (currentTime > nextRefreshTime) {
                nextRefreshTime = currentTime + FRAME_TIME;
            }
        }
    }
}

void Server::disconnectPlayer(int clientId){
    close(clientIdToSocket[clientId]); // Fermer la connexion
    clientGameRoomId.erase(clientId); // Supprimer l'ID de la salle de jeu   
    clientIdToSocket.erase(clientId); // Supprimer le socket du client
    clientStates.erase(clientId); // Supprimer l'état des menus du client
}

void Server::stop() {
    if (serverSocket != -1) {
        shutdown(serverSocket, SHUT_RDWR);
        close(serverSocket);
        serverSocket = -1;
    }
}

void Server::sendMenuToClient(int clientSocket, const std::string& screen) {
    send(clientSocket, screen.c_str(), screen.size(), 0);
}

void Server::returnToMenu(int clientSocket, int clientId, MenuState state, const std::string& message, int sleepTime){
    if (!message.empty()) {
        sendMenuToClient(clientSocket, menu.displayMessage(message));
        sleep(sleepTime);
    }
    clientStates[clientId] = state;
    handleMenu(clientSocket, clientId, "", true); // Refresh the menu
}

void Server::handleMenu(int clientSocket, int clientId, const std::string& action, bool refreshMenu) {
    auto currentClient = clientPseudo[clientId];
    auto roomName = roomToManage[clientId];
    switch (clientStates[clientId]) {
        case MenuState::Welcome:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getMainMenu0()); 
            else keyInputWelcomeMenu(clientSocket, clientId, action);
            break;
        case MenuState::RegisterPseudo:
            if (!refreshMenu)keyInputRegisterPseudoMenu(clientSocket, clientId, action);
            break;
        case MenuState::RegisterPassword:
            if (!refreshMenu) keyInputRegisterPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::LoginPseudo:
            if (!refreshMenu) keyInputLoginPseudoMenu(clientSocket, clientId, action);
            break;
        case MenuState::LoginPassword:
            if (!refreshMenu) keyInputLoginPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::Main:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getMainMenu1());
            else keyInputMainMenu(clientSocket, clientId, action);
            break;
        case MenuState::classement:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getRankingMenu(userManager.getRanking()));
            else keyInputRankingMenu(clientSocket, clientId, action);
            break;
        case MenuState::Team:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getTeamsMenu());
            else keyInputTeamsMenu(clientSocket, clientId, action);
            break;
        case MenuState::CreateTeam:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getCreateChatRoomMenu());
            else keyInputCreateChatRoom(clientSocket, clientId, action);
            break;
        case MenuState::JoinTeam:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getJoinChatRoomMenu());
            else keyInputJoinChatRoom(clientSocket, clientId, action);
            break;
        case MenuState::TeamsInvitation:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getInvitationsRoomsMenu(chatRoomsManage.getTeamsInvitaionForUser(currentClient)));
            else keyInputTeamsInvitation(clientSocket, clientId, action);
            break;
        case MenuState::ManageTeams:
            if(refreshMenu)sendMenuToClient(clientSocket, menu.getManageTeamsMenu(chatRoomsManage.getChatRoomsForUser(sockToPseudo[clientSocket])));
            else keyInputManageTeams(clientSocket, clientId, action);
            break;
        case MenuState::ManageTeam:
            if(refreshMenu)  sendMenuToClient(clientSocket, menu.getManageTeamMenu(chatRoomsManage.isAdmin(sockToPseudo[clientSocket],roomToManage[clientId]), chatRoomsManage.getAdmins(roomToManage[clientId]).size() == 1));
            else keyInputManageTeam(clientSocket, clientId, action);
            break;
        case MenuState::ListTeamMembres:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getListeMembers(chatRoomsManage.getMembers(roomName)));
            else keyInputListMembres(clientSocket, clientId, action);
            break;
        case MenuState::AddMembre:
            if(!refreshMenu) keyInputAddMembre(clientSocket, clientId, action);
            break;
        case MenuState::AddAdmin:
            if(!refreshMenu) keyInputAddAdmin(clientSocket, clientId, action);
            break;
        case MenuState::RoomRequestList:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getListeRequests(chatRoomsManage.getClientPending(roomName)));
            else keyInputRequestList(clientSocket, clientId, action);
            break;
        case MenuState::ConfirmDeleteRoom:
            if(!refreshMenu) keyInputConfirmDeleteRoom(clientSocket, clientId, action);
            break;
        case MenuState::ConfirmQuitRoom:
            keyInputQuitRoom(clientSocket, clientId, action);
            break;
        case MenuState::TeamChat:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getChooseContactMenu(chatRoomsManage.getChatRoomsForUser(sockToPseudo[clientSocket]), false));
            else keyInputChatTeam(clientSocket, clientId, action);
            break;
        case MenuState::Friends:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getFriendMenu());
            else keyInputFriendsMenu(clientSocket, clientId, action);
            break;
        case MenuState::AddFriend:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getAddFriendMenu());
            else keyInputAddFriendMenu(clientSocket, clientId, action);
            break;
        case MenuState::FriendList:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getFriendListMenu(userManager.getFriendList(currentClient)));
            else keyInputManageFriendlist(clientSocket, clientId, action);
            break;
        case MenuState::FriendRequestList:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getRequestsListMenu(userManager.getRequestList(currentClient)));
            else keyInputManageFriendRequests(clientSocket, clientId, action);
            break;
        case MenuState::PrivateChat:
            if (refreshMenu) sendMenuToClient(clientSocket, menu.getChooseContactMenu(userManager.getFriendList(clientPseudo[clientId]), true));
            else keyInputPrivateChat(clientSocket, clientId, action);
            break;
        case MenuState::JoinOrCreateGame:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
            else keyInputJoinOrCreateGameMenu(clientSocket, clientId, action);
            break;
        case MenuState::JoinGame:
            if(refreshMenu){
                std::cout << "" << std::endl;
                std::vector<std::vector<std::string>> invitations = userManager.getListGameRequest(currentClient);
                sendMenuToClient(clientSocket, menu.getGameRequestsListMenu(invitations));
            }
            keyInputChoiceGameRoom(clientSocket, clientId, action);
            break;
        case MenuState::CreateGame:
            if(refreshMenu) sendMenuToClient(clientSocket, menu.getGameModeMenu());
            else keyInputChooseGameModeMenu(clientSocket, clientId, action);
            break;
        case MenuState::GameOver:
            keyInputGameOverMenu(clientSocket, clientId, action);
            break;
        case MenuState::Settings:
            keyInputLobbySettingsMenu(clientSocket, clientId, action);
            break;
        case MenuState::Help:
            keyInputHelpMenu(clientSocket, clientId, action);
            break;
        case MenuState::Play:
            sendInputToGameRoom(clientId, action);
            break;
        default:
            std::cerr << "Erreur: État de menu non géré." << std::endl;
            break;
    }
}


void Server::keyInputChooseGameModeMenu(int clientSocket, int clientId, const std::string& action){
    if(action == "1"){
        //Endless
        std::cout << "Client #" << clientId << " a sélectionné Endless." << std::endl;
        clientStates[clientId] = MenuState::Play;
        this->keyInputCreateGameRoom(clientId, GameModeName::Endless);

    }
    else if(action == "2"){
        //Duel
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu(2, "Duel", 1));
        this->keyInputCreateGameRoom(clientId, GameModeName::Duel);
        
    }
    else if(action == "3"){
        //classic
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu(3, "Classic", 1));
        this->keyInputCreateGameRoom(clientId, GameModeName::Classic);
    }
    else if(action == "4"){
        //royal competition
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu(3, "Royal Competition", 1));
        this->keyInputCreateGameRoom(clientId, GameModeName::Royal_Competition);
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

    std::vector<std::string> requests = userManager.getRequestList(currentUser);
    if (requests.empty()) {
        return;
    }

    if (action == "accept.all" || action == "reject.all") {
        bool accept = (action == "accept.all");
        for (const std::string& friend_request : requests) {
            accept ? userManager.acceptFriendRequest(currentUser, friend_request)
                   : userManager.rejectFriendRequest(currentUser, friend_request);
        }

        returnToMenu(clientSocket, clientId, MenuState::Friends,
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
        //if (!userManager.isPendingRequest(friend_request, currentUser)) {
        //    returnToMenu(clientSocket, clientId, MenuState::Friends, "Erreur : Aucune demande d'ami en attente de '" + friend_request + "'.");
        //    return;
        //}

        isAccept ? userManager.acceptFriendRequest(currentUser, friend_request)
                 : userManager.rejectFriendRequest(currentUser, friend_request);

        returnToMenu(clientSocket, clientId, MenuState::FriendRequestList,
            (isAccept ? "Demande d'ami acceptée avec " : "Demande d'ami rejetée pour '") + friend_request + "'.");
        return;
    }

    returnToMenu(clientSocket, clientId, MenuState::Friends, 
                 "Erreur : Format invalide. Utilisez 'accept.pseudo', 'accept.all', 'reject.pseudo' ou 'reject.all'.");
}


void Server::keyInputManageFriendlist(int clientSocket, int clientId, const std::string& action) {
    std::string currentUser = clientPseudo[clientId];

    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
        return;
    }

    std::vector<std::string> friends = userManager.getFriendList(currentUser);
    if (friends.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::Main, "Vous n'avez aucun ami dans votre liste.");
        return;
    }

    if (action == "del.all") {
        for (const std::string& friend_name : friends) {
            userManager.deleteFriend(currentUser, friend_name);
        }

        returnToMenu(clientSocket, clientId, MenuState::Friends, "Tous vos amis ont été supprimés.");
        return;
    }

    std::string prefix_del = "del.";
    if (action.rfind(prefix_del, 0) == 0) {  // Vérifie si action commence par "del."
        std::string friend_name = action.substr(prefix_del.size());

        if (!userManager.areFriends(currentUser, friend_name)) {
            returnToMenu(clientSocket, clientId, MenuState::Friends, "Erreur : '" + friend_name + "' n'est pas dans votre liste d'amis.");
            return;
        }

        userManager.deleteFriend(currentUser, friend_name);
        returnToMenu(clientSocket, clientId, MenuState::Friends, friend_name + " a été supprimé de votre liste d'amis.");
        return;
    }

    returnToMenu(clientSocket, clientId, MenuState::FriendRequestList, "Erreur : Format invalide. Utilisez 'del.pseudo' ou 'del.all'.");
}

void Server::keyInputFriendsMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend);
    }
    else if (action == "2") {
        returnToMenu(clientSocket, clientId, MenuState::FriendList);
    }
    else if (action == "3") {
        returnToMenu(clientSocket, clientId, MenuState::FriendRequestList);
    }
    else if (action == "4") {
        returnToMenu(clientSocket, clientId, MenuState::PrivateChat);
    }else if( action == "5"){
        returnToMenu(clientSocket, clientId, MenuState::Main);
    }
}

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

    // Vérifier si l'utilisateur existe
    if (!userManager.userExists(friend_request)) {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend, "Erreur : L'utilisateur " + friend_request + " n'existe pas.");
        return;
    }

    // Vérifier si l'utilisateur essaie de s'ajouter lui-même
    if (friend_request == currentUser) {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend, "Erreur : Vous ne pouvez pas vous ajouter vous-même.");
        return;
    }

    // Vérifier si les deux utilisateurs sont déjà amis
    if (userManager.areFriends(currentUser, friend_request)) {
        returnToMenu(clientSocket, clientId, MenuState::Friends, "Erreur : Vous êtes déjà ami avec " + friend_request + ".");
        return;
    }


    // Envoyer la demande d'ami
    if(!userManager.sendFriendRequest(currentUser, friend_request))
         returnToMenu(clientSocket, clientId, MenuState::Friends, "Demande d'ami déja existe!");
    else returnToMenu(clientSocket, clientId, MenuState::Friends, "Demande d'ami'Demande d'ami envoyée à " + friend_request + ". Veuiller consulter la listes des amis pour voir si la demande a été acceptée.");
    
}

void Server::handleChat(int clientSocket, int clientId, json& receivedData){
    // Handle chat messages
    auto receiver = receiverOfMessages[clientId];
    bool isRoom = chatRoomsManage.checkroomExist(receiver);
    std::map<std::string, int> receivers;
    if (isRoom) {
        auto members = chatRoomsManage.getMembers(receiver);
        for (const auto& member : members) {
            if (pseudoTosocket.find(member) != pseudoTosocket.end() && member != clientPseudo[clientId]) {
                receivers[member] = pseudoTosocket[member];
            } else {
                std::cerr << "Member " << member << " not found in pseudoTosocket." << std::endl;
            }
        }
    } else {
        receivers[receiver] = pseudoTosocket[receiver];
    }

    if(!chat.processClientChat(clientSocket, clientPseudo[clientId], receivers, receivedData)){
        if(isRoom){
            clientStates[clientId] = MenuState::Team;
        }else{
            clientStates[clientId] = MenuState::Friends;
        }
    }else{
        if(isRoom){
            chatRoomsManage.saveMessageToRoom(clientPseudo[clientId], receiver, receivedData[jsonKeys::MESSAGE]);
        }
    }
}

void Server::keyInputPrivateChat(int clientSocket, int clientId, const std::string& action){
    if(action == "/back"){
        returnToMenu(clientSocket, clientId, MenuState::Main);
    }
    else if(userManager.userNotExists(action)){
        returnToMenu(clientSocket, clientId, MenuState::PrivateChat, "User does not exist.");
    }else if( !userManager.areFriends(sockToPseudo[clientSocket], action)){
        returnToMenu(clientSocket, clientId, MenuState::PrivateChat, "You are not friends with this user.");
    }else{
        receiverOfMessages[clientId] = action;                                  //stocker le pseudo du destinataire
        clientStates[clientId] = MenuState::chat;
        sendChatModeToClient(clientSocket);                                     //envoyer le mode de chat au client
        sleep(0.1); // Attendre un peu pour s'assurer que le client a reçu le mode de chat
        chat.sendOldMessages(clientSocket, sockToPseudo[clientSocket], action); //envoyer les anciens messages
    }
}
void Server::keyInputChatTeam(int clientSocket, int clientId, const std::string& action){
    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Team);
        return;
    }

    if (chatRoomsManage.checkroomExist(action)) { // Vérifie si la room existe
        if (chatRoomsManage.isClient(sockToPseudo[clientSocket], action)) {
            receiverOfMessages[clientId] = action;
            clientStates[clientId] = MenuState::chat;
            sendChatModeToClient(clientSocket);
            sleep(0.1); // Attendre un peu pour s'assurer que le client a reçu le mode de chat
            //chatRoomsManage.sendOldMessages(clientSocket, action); //envoyer les anciens messages
        } else {
            returnToMenu(clientSocket, clientId, MenuState::Team, "Vous n'êtes pas membre de cette room.");
        }
    } else {
        returnToMenu(clientSocket, clientId, MenuState::Team, "La room spécifiée n'existe pas.");
    }
}


void Server::keyInputCreateGameRoom(int clientId, GameModeName gameMode) { //création de la gameRoom
    std::lock_guard<std::mutex> lock(gameRoomsMutex);

    std::cout << "Création de la GameRoom pour le client #" << clientId << " avec le mode " << static_cast<int>(gameMode) << "." << std::endl;
    
    int gameRoomIndex = gameRoomIdCounter.fetch_add(1);
    gameRooms[gameRoomIndex] = std::make_shared<GameRoom>(gameRoomIndex, clientId, gameMode);
    clientGameRoomId[clientId] = gameRoomIndex;
    
    std::thread loopgame(&Server::loopGame, this, clientId);
    loopgame.detach();
}


std::string Server::trim(const std::string& s) {
    const char* whitespace = " \t\n\r";
    std::size_t start = s.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    std::size_t end = s.find_last_not_of(whitespace);
    return s.substr(start, end - start + 1);
}

void Server::deleteGameRoom(int roomId, const std::vector<int> players) {
    for (auto player : players)
        clientGameRoomId[player] = -1;
    gameRooms.erase(roomId);
    gameRoomIdCounter.fetch_sub(1);
    std::cout << "GameRoom #" << roomId << " deleted." << std::endl;
}

void Server::sendInputToGameRoom(int clientId, const std::string& action) {
    gameRooms[clientGameRoomId[clientId]]->input(clientId, action);
}

void Server::letPlayersPlay(const std::vector<int>& players) {
    for (int player : players)
        clientStates[player] = MenuState::Play;
}

void Server::loopGame(int ownerId) {
    auto gameRoom = gameRooms[clientGameRoomId[ownerId]];
    std::vector<int> players;

    if (gamePreparation(ownerId, gameRoom)){
        players = gameRoom->getPlayers();
        int maxPlayers = gameRoom->getMaxPlayers();
        gameRoom->createGames();
        int countGameOvers = 0;
        letPlayersPlay(players);
        gameRoom->startGame();
        
        while (gameRoom->getInProgress()) {
            countGameOvers = 0;
            for (auto player : players) {
                try {
                    if (gameRoom->getGameIsOver(player))
                    countGameOvers++;
                    else {
                        gameRoom->updatePlayerGame(player);
                        if (gameRoom->getNeedToSendGame(player)) {
                            sendGameToPlayer(player, clientIdToSocket[player], gameRoom);
                            gameRoom->setNeedToSendGame(false, player);
                        }
                    }
                    if (gameRoom->getAmountOfPlayers() != maxPlayers - countGameOvers) 
                    gameRoom->setAmountOfPlayers(maxPlayers - countGameOvers);
                } catch (const std::exception& e) {
                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                    disconnectPlayer(player);
                    gameRoom->removePlayer(player);
                    std::cerr << "Erreur lors de la mise à jour du jeu pour le joueur #" << player << ": " << e.what() << std::endl;
                }
            }
        }

        std::string message = "GAME OVER";
        for (auto player : players) {
            GameModeName gameMode = gameRoom->getGameModeName();
            try {
                clientStates[player] = MenuState::GameOver;
                if (gameMode != GameModeName::Endless)
                    message = (!gameRoom->getGameIsOver(player)) ? "YOU WIN !!" : "GAME OVER";
                else
                    userManager.updateHighScore(clientPseudo[player], gameRoom->getScoreValue());
                sendMenuToClient(clientIdToSocket[player], menu.getEndGameMenu(message));
            } catch (const std::exception& e) {
            players.erase(std::remove(players.begin(), players.end(), player), players.end());
            disconnectPlayer(player);
            std::cerr << "Erreur lors de l'envoi du message de fin de jeu au joueur #" << player << ": " << e.what() << std::endl;
            }
        }
    
    }

    {
    std::lock_guard<std::mutex> lock(gameRoomsMutex);
    int roomId = gameRoom->getRoomId();
    std::cout << "GameRoom #" << roomId << " ended." << std::endl;
    if (players.empty())
        players = gameRoom->getPlayers();
    deleteGameRoom(roomId, players);
    }
}

bool Server::gamePreparation(int ownerId, std::shared_ptr<GameRoom> gameRoom){
    if (gameRoom->getGameModeName() != GameModeName::Endless) {
        while(!(gameRoom->getSettingsDone() || gameRoom->getOwnerQuit()))
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (gameRoom->getOwnerQuit()) {
            gameRoom->endGame();
            for (auto player : gameRoom->getPlayers()) {
                if (player != ownerId){
                    clientStates[player] = MenuState::GameOver;
                    sendMenuToClient(clientIdToSocket[player], menu.getEndGameMenu("GAME END"));
                }
                else {
                    clientStates[player] = MenuState::JoinOrCreateGame;
                    sendMenuToClient(clientIdToSocket[player], menu.getJoinOrCreateGame());
                }
            }
            return false;
        }
    }
    return true;
}


void Server::keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {  // Se connecter
        clientStates[clientId] = MenuState::LoginPseudo;
        std::cout << "hello" << std::endl;
        sendMenuToClient(clientSocket, menu.getLoginMenu1());
    }
    else if (action == "2") { // Créer un compte
        clientStates[clientId] = MenuState::RegisterPseudo;
        sendMenuToClient(clientSocket, menu.getRegisterMenu1());
    }
    else if (action == "3") {
        // Quitter
        std::cout << "Client #" << clientId << " déconnecté." << std::endl;
        // Fermer la connexion
        disconnectPlayer(clientId);
        //return to terminal
        return;
    }
    else {
        sendMenuToClient(clientSocket, menu.getMainMenu0());
    }
}

void Server::keyInputRegisterPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    if (userManager.userNotExists(action)){
        // Si le pseudo n'existe pas, on stock en tmp
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::RegisterPassword;
        sendMenuToClient(clientSocket, menu.getRegisterMenu2());
    }
    else {
        returnToMenu(clientSocket,clientId, MenuState::RegisterPseudo, "pseudo already existe");
        sendMenuToClient(clientSocket, menu.getRegisterMenu1());
    }
}

void Server::keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    userManager.registerUser(clientPseudo[clientId], action);
    sockToPseudo[clientSocket] = clientPseudo[clientId];
    pseudoTosocket[clientPseudo[clientId]] = clientSocket;
    clientStates[clientId] = MenuState::Main;
    sendMenuToClient(clientSocket, menu.getMainMenu1());
}


void Server::keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    std::cout << "Pseudo: " << action << std::endl;
    if (!userManager.userNotExists(action)) { // Si le pseudo existe
        std::cout << "Pseudo existe" << std::endl;
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::LoginPassword;
        sendMenuToClient(clientSocket, menu.getLoginMenu2());
    } 
    else {
        std::cout << "Pseudo n'existe pas" << std::endl;
        // si pseudo n'existe pas on annule et on retourne à l'étape 1 (dc dmd de pseudo)
        clientStates[clientId] = MenuState::RegisterPseudo;
        sendMenuToClient(clientSocket, menu.getLoginMenuFailed1());
    }
}


void Server::keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    if (userManager.authenticateUser(clientPseudo[clientId], action)) { // Si le mot de passe est correct
        clientStates[clientId] = MenuState::Main;
        pseudoTosocket[clientPseudo[clientId]] = clientSocket;
        sockToPseudo[clientSocket] = clientPseudo[clientId];
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    } 
    else {
        // Si le mot de passe est incorrect, on retourne à l'étape 2 (dmd de mdp)
        sendMenuToClient(clientSocket, menu.getLoginMenuFailed2());
    }
}


void Server::keyInputMainMenu(int clientSocket, int clientId, const std::string& action) {
    (void) clientSocket;
    if (action == "1") { // créer une gameRoom (choisir s'il veut créer sa partie ou rejoindre une partie)
        clientStates[clientId] = MenuState::JoinOrCreateGame;
    }
    else if (action == "2") {
        clientStates[clientId] = MenuState::Friends;
    }
    else if (action == "3") {
        // Classement
        clientStates[clientId] = MenuState::classement;
    }
    else if (action == "4") {
        // Teams
        clientStates[clientId] = MenuState::Team;
    }
    else if (action == "5") {
        // Retour à l'écran précédent
        clientStates[clientId] = MenuState::Welcome;
    }
}


void Server::keyInputJoinOrCreateGameMenu(int clientSocket, int clientId, const std::string& action) {
    (void)clientSocket;
    if (action == "1") {
        clientStates[clientId] = MenuState::CreateGame;
        }
    else if (action == "2") {
        clientStates[clientId] = MenuState::JoinGame;
        // Rejoindre une partie
    }
    else if (action == "3") {
        clientStates[clientId] = MenuState::Main;
    }
}


//Définir les actions possibles dans le chat


void Server::keyInputTeamsMenu(int clientSocket, int clientId, const std::string& action) {
    if(action == "1") {
        clientStates[clientId] = MenuState::CreateTeam;
    }
    else if(action == "2") {
        clientStates[clientId] = MenuState::JoinTeam;
    }
    else if(action == "3"){
        clientStates[clientId] = MenuState::TeamsInvitation;
    }
    else if(action == "4") {
        clientStates[clientId] = MenuState::ManageTeams;
    }
    else if(action == "5") {
        //chat Room
        clientStates[clientId] = MenuState::TeamChat;
    }
    else if(action == "6") {
        returnToMenu(clientSocket, clientId, MenuState::Main);
    }
}


void Server::keyInputCreateChatRoom(int clientSocket, int clientId, const std::string& action) {
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Team);
    }
    else if(chatRoomsManage.checkroomExist(action)){
        returnToMenu(clientSocket, clientId, MenuState::Team, "Room name already exists.");
    }
    else{
        //roomToManage[clientId] = action;
        chatRoomsManage.createTeam(action, sockToPseudo[clientSocket]);
        returnToMenu(clientSocket, clientId, MenuState::Team, "Room created successfully.");
    }

}

void Server::keyInputJoinChatRoom(int clientSocket, int clientId, const std::string& action) {
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Team);
    }
    else if(!chatRoomsManage.checkroomExist(action)){
        returnToMenu(clientSocket, clientId, MenuState::Team, "Room name does not exist.");
    }
    else if(chatRoomsManage.isClient(action, sockToPseudo[clientSocket])){
        returnToMenu(clientSocket, clientId, MenuState::Team, "You are already a member in this room");
    }
    else if(chatRoomsManage.isPendingReq(action, sockToPseudo[clientSocket])){
        returnToMenu(clientSocket, clientId, MenuState::Team, "You already sent a request to join this room");
    }
    else{
        chatRoomsManage.joinTeam(sockToPseudo[clientSocket], action);
        returnToMenu(clientSocket, clientId, MenuState::Team, "Request to join room sent succesfully.");
    }
}

void Server::keyInputTeamsInvitation(int clientSocket, int clientId, const std::string& action){
    std::vector<std::string> requests;
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Team);
    }
    else if(action.rfind("accept.", 0) == 0 || action.rfind("refuse.", 0) == 0){
        auto option = action.substr(0, 7);
        auto roomName = action.substr(7);
        auto client = clientPseudo[clientId];
        //si la room est supprimé
        if(!chatRoomsManage.checkroomExist(roomName)){
            returnToMenu(clientSocket, clientId, MenuState::TeamsInvitation, "Room name does not exist.");
        }
        else{
            if(option == "accept."){
                chatRoomsManage.acceptClientRequest(client , roomName);
                returnToMenu(clientSocket, clientId, MenuState::TeamsInvitation, "Request to join room accepted.");
            }
            else{
                chatRoomsManage.refuseClientRequest(client, roomName);
                returnToMenu(clientSocket, clientId, MenuState::TeamsInvitation, "Request to join room refused.");
            }
        }
    }
}


void Server::keyInputManageTeams(int clientSocket, int clientId, const std::string& action) {
    //std::vector<std::string> myRooms = chat->getMyRooms(sockToPseudo[clientSocket]);
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::Team);
    }
    else if(!chatRoomsManage.checkroomExist(action)){ 
        returnToMenu(clientSocket, clientId, MenuState::Team, "Room name does not exist.");
    } 
    else {
        roomToManage[clientId] = action;
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
}


void Server::keyInputManageTeam(int clientSocket, int clientId, const std::string& action) {
    auto roomName = roomToManage[clientId];
    if(action == "1"){
            returnToMenu(clientSocket, clientId, MenuState::ListTeamMembres);
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
            }
            //Supression de la room
            else if(action == "5") {
                if (chatRoomsManage.getMembers(roomName).size() == 1) {
                    clientStates[clientId] = MenuState::ConfirmDeleteRoom;
                    sendMenuToClient(clientSocket, menu.getDeleteRoomConfirmationMenu(roomName));

                } else {
                    clientStates[clientId] = MenuState::ConfirmQuitRoom;
                    sendMenuToClient(clientSocket, menu.getQuitRoomConfirmationMenu(roomName));
                }
            }else if(action == "6") {
                returnToMenu(clientSocket, clientId, MenuState::ManageTeams);//retour
            }
        }
        else {
            if(action == "2") {
                returnToMenu(clientSocket, clientId, MenuState::ConfirmQuitRoom);
            }else if(action == "3") {
                returnToMenu(clientSocket, clientId, MenuState::ManageTeams);//retour
            }
        }
}


void Server::keyInputListMembres(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
    else if(action.rfind("del.", 0) == 0){
        std::string pseudo = action.substr(4);
        if (chatRoomsManage.isClient(pseudo,roomName)){
            chatRoomsManage.removeClient(pseudo,roomName);
            returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User removed successfully.");
        }else if (chatRoomsManage.removeClient(roomName, pseudo)){
            returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User removed successfully.");
        }
    }
    
    else {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }

}

void Server::keyInputAddMembre(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];

    if (action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
    else if (chatRoomsManage.isClient(action, roomName)) { 
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User is already in the room.");
    }
    else if (userManager.userNotExists(action)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User does not exist.");
    }
    else if (chatRoomsManage.sendInvitationToClient(action, roomName)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "Request sent successfully.");
    }
    else {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User has already sent a Request to Join Room !");
    }
}


void Server::keyInputAddAdmin(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
    else if(chatRoomsManage.isAdmin(action, roomName)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User is already an admin.");
    }
    else if(!chatRoomsManage.isClient(action, roomName)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User is not in the room.");
    }
    else {
        chatRoomsManage.addAdmin(action, roomName);
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User is now an admin.");
    }
}

void Server::keyInputRequestList(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
    else if(action.rfind("accept.", 0) == 0){
        std::string pseudo = action.substr(7);
        if (chatRoomsManage.isPendingReq(pseudo, roomName )){
            chatRoomsManage.acceptClientRequest(pseudo, roomName);
            returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User added successfully.");
        }
        else if(chatRoomsManage.isClient(pseudo, roomName)){
            returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User is already in the room.");
        }
    }
    else if(action.rfind("reject.", 0) == 0){
        std::string pseudo = action.substr(7);
        if (chatRoomsManage.isPendingReq(pseudo, roomName)){
            chatRoomsManage.refuseClientRequest(pseudo, roomName);
            returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User removed successfully.");
        }
        else if(chatRoomsManage.isClient(pseudo, roomName)){
            returnToMenu(clientSocket, clientId, MenuState::ManageTeam, "User is already in the room.");
        }
    }
    else {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
}

void Server::keyInputQuitRoom(int clientSocket, int clientId, const std::string& action){
    std::string roomName = roomToManage[clientId];
    std::string userPseudo = sockToPseudo[clientSocket];

    if(action == "/back") {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
        return;
    }
    bool isAdmin = chatRoomsManage.isAdmin(userPseudo, roomName);
    if (action == "oui"){
        if (isAdmin) {
            if (chatRoomsManage.getAdmins(roomName).size() > 1) {
                chatRoomsManage.removeAdmin(userPseudo, roomName);
                returnToMenu(clientSocket, clientId, MenuState::ManageTeams, "You have left the room.");
            } else {
                chatRoomsManage.deleteChatRoom(roomName);
                returnToMenu(clientSocket, clientId, MenuState::ManageTeams, "Room deleted successfully.");
            }
        } else {
            chatRoomsManage.removeClient(userPseudo, roomName);
            returnToMenu(clientSocket, clientId, MenuState::ManageTeams, "You have left the room.");
        }        
    }else{
        //retour au menu de gestion de la room (si on a pas quitté/ supprimé la room)
        returnToMenu(clientSocket, clientId, MenuState::ManageTeam);
    }
}

void Server::keyInputConfirmDeleteRoom(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if (action == "oui") {
        chatRoomsManage.deleteChatRoom(roomName);
        returnToMenu(clientSocket, clientId, MenuState::ManageTeams, "Room '" + roomName + "' supprimée avec succès.");
    } else {
        returnToMenu(clientSocket, clientId, MenuState::ManageTeams, "Suppression de la room est refuseee.");
    }
}

//Définir les actions possibles dans le classement
void Server::keyInputRankingMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "/back") { 
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




void Server::sendGameToPlayer(int clientId, int clientSocket, std::shared_ptr<GameRoom> gameRoom) {
    std::shared_ptr<Game> game = gameRoom->getGame(clientId);
    json message, tetraminos = game->tetraminoToJson();

    message[jsonKeys::SCORE] = game->scoreToJson();
    message[jsonKeys::GRID] = game->gridToJson();
    message[jsonKeys::TETRA_PIECE] = tetraminos[jsonKeys::TETRA_PIECE]; // Ajout du tétrimino dans le même message
    message[jsonKeys::NEXT_PIECE] = tetraminos[jsonKeys::NEXT_PIECE];

    message[jsonKeys::MESSAGE_CIBLE] = gameRoom->messageToJson(clientId);
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


////////////////Ce qui suit est pour les invitations de jeu/////////////////
//************************************************************************ */

/*
To do
*/
void Server::keyInputSendGameRequestMenu(int clientSocket, int clientId, std::string receiver, std::string status) {
    //### GÉRER L'INVITATION D'UN VIEWER
    std::cout << "Sending game request to: " << receiver << " with status: " << status << std::endl;
    std::string currentUser = clientPseudo[clientId];

    if(receiver == currentUser){
        sendMenuToClient(clientSocket, menu.displayMessage("Erreur : Vous ne pouvez pas vous envoyer une invitation de jeu."));
        sleep(2);
        return;
    }
    if(!userManager.areFriends(currentUser,receiver)){
        sendMenuToClient(clientSocket, menu.displayMessage("Erreur : Vous n'êtes pas ami avec "+receiver+"."));
        sleep(2);
        return;
    }
    int gameRoomId = clientGameRoomId[clientId];
    userManager.sendInvitationToFriend(gameRoomId, currentUser, receiver,status);
    sendMenuToClient(clientSocket, menu.displayMessage("Request Sent To "+receiver+"."));
    sleep(2);

}


void Server::keyInputHelpMenu(int clientSocket, int clientId, const std::string& action) {
    auto gameRoom = gameRooms[clientGameRoomId[clientId]];
    if (action.find("/back") == 0){
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu(getMaxPlayers(clientId), getMode(clientId), getAmountOfPlayers(clientId), gameRoom->getSpeed(), gameRoom->getEnergyLimit()));
    }
    else {
        bool isRC = gameRoom->getGameModeName() == GameModeName::Royal_Competition;
        bool canEditMaxPlayer = gameRoom->getGameModeName() == GameModeName::Classic || isRC;
        keyInputLobbySettingsMenu(clientSocket, clientId, action);
        sendMenuToClient(clientSocket, menu.getHelpMenu(isRC, canEditMaxPlayer));
    }
}

void Server::keyInputLobbySettingsMenu(int clientSocket, int clientId, const std::string& action) {
    std::string status, receiver;
    // Vérifier si l'action commence par \invite
    if (action.find("/invite") == 0) {
        // Extraire le statut et le receiver de l'action
        extractDataBetweenSlashes("/invite", action, status, receiver);

        // Appeler la méthode pour gérer l'envoi de la demande de jeu
        keyInputSendGameRequestMenu(clientSocket, clientId, receiver, status);        
    }

    else if (action.find("/friend") == 0){
        // Extraire le statut et le receiver de l'action
        extractDataBetweenSlashes("/friend", action, status, receiver);     
        if (status == "add")
            keyInputAddFriendMenu(clientSocket, clientId, receiver);
        else if (status == "delete")
            keyInputManageFriendlist(clientSocket, clientId, "del." + receiver);
        else if (status == "ban")
            keyInputManageFriendlist(clientSocket, clientId, "ban." + receiver);
        else if (status == "unban")
            keyInputManageFriendlist(clientSocket, clientId, "unban." + receiver); 
    }

    auto gameRoom = gameRooms[clientGameRoomId[clientId]];

    if (action.find("/help") == 0){
        clientStates[clientId] = MenuState::Help;
        bool isRC = gameRoom->getGameModeName() == GameModeName::Royal_Competition;
        bool canEditMaxPlayer = gameRoom->getGameModeName() == GameModeName::Classic || isRC;
        sendMenuToClient(clientSocket, menu.getHelpMenu(isRC, canEditMaxPlayer));
        return;
    }
    
    if (action.find("/back") == 0){
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
    
    if ((gameRoom != nullptr) && (!gameRoom->getInProgress()) && (clientStates[clientId] == MenuState::Settings))   //refresh le Lobby
        sendMenuToClient(clientSocket, menu.getLobbyMenu(getMaxPlayers(clientId), getMode(clientId), getAmountOfPlayers(clientId), gameRoom->getSpeed(), gameRoom->getEnergyLimit()));
}

void Server::extractDataBetweenSlashes(const std::string& toFind, const std::string& action, std::string& status, std::string& receiver){
        // Extraire le statut et le receiver de l'action
        std::size_t firstBackSlash = action.find('/', std::string(toFind).size());  // Trouver le premier \ après \invite
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
        status = action.substr(firstBackSlash + 1, secondBackSlash - firstBackSlash - 1); 
        
        // Extraire le receiver (nom du joueur)
        receiver = action.substr(secondBackSlash + 1); 
}


void Server::keyInputChoiceGameRoom(int clientSocket, int clientId, const std::string& action){
    if (action.find("accept.") == 0){
        std::size_t pos = action.find(".");
        std::string number = action.substr(pos+1, action.size());
        int roomNumber = std::stoi(number);
        userManager.acceptGameInvitation(roomNumber, clientPseudo[clientId]);
        clientGameRoomId[clientId] = roomNumber;
        auto gameRoom = gameRooms[roomNumber];
        gameRoom->addPlayer(clientId);
        clientStates[clientId] = MenuState::Settings;
        
        for (const auto& cId : gameRooms[roomNumber]->getPlayers())
            sendMenuToClient(pseudoTosocket[clientPseudo[cId]], menu.getLobbyMenu(getMaxPlayers(cId), getMode(cId), getAmountOfPlayers(cId), gameRoom->getSpeed(), gameRoom->getEnergyLimit()));

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

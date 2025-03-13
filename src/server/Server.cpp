#include "Server.hpp"
#include "../common/json.hpp"
#include "../common/jsonKeys.hpp"
#include <ncurses.h>
#include <unistd.h>
#include <csignal>
#include <netinet/in.h>
#include <fstream>
#include <iostream>
#include <algorithm> // for std::find


Server::Server(int port) 
    : port(port), serverSocket(-1), clientIdCounter(0){
        userManager = std::make_unique<UserManager>("./users.txt");
        friendList = std::make_unique<FriendList>();
    }


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

    while (true) {
        
        if(getRunningChat(clientSocket)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Attendre 100ms avant de vérifier à nouveau
            continue;
        }

        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Client #" << clientId << " déconnecté." << std::endl;
            if (clientStates[clientId] == MenuState::Play) {
                int roomId = clientGameRoomId[clientId];
                if ((roomId != -1) && (gameRooms[roomId].get() != nullptr)) {
                    gameRooms[roomId]->setGameIsOver(clientId);
                    if (!gameRooms[roomId]->getInProgress())
                        deleteGameRoom(roomId);
                }
            }
            clientStates.erase(clientId); // Supprimer l'état des menus du client
            close(clientSocket); // Fermer la connexion
            break;
        }

        try {
            json receivedData = json::parse(buffer);

            if (!receivedData.contains(jsonKeys::ACTION) || !receivedData[jsonKeys::ACTION].is_string()) {
                std::cerr << "Erreur: 'action' manquant ou invalide dans le JSON reçu." << std::endl;
                return;
            }

            std::string action = receivedData[jsonKeys::ACTION];
            handleMenu(clientSocket, clientId, action); // Gérer l'action du client
        } catch (json::parse_error& e) {
            std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
        }
    }
}

void Server::handleMenu(int clientSocket, int clientId, const std::string& action) {
    if (clientStates[clientId] == MenuState::Welcome) {
        keyInputWelcomeMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::RegisterPseudo) {
        keyInputRegisterPseudoMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::RegisterPassword) {
        keyInputRegisterPasswordMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::LoginPseudo) {
        keyInputLoginPseudoMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::LoginPassword) {
        keyInputLoginPasswordMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::Main) {
        keyInputMainMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::classement) {
        keyInputRankingMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::chat) {
        keyInputChatMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::CreateRoom) {
        keyInputCreateChatRoom(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::JoinRoom) {
        keyInputJoinChatRoom(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::ManageRooms) {
        keyInputManageMyRooms(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::ManageRoom) {
        keyInputManageRoom(clientSocket, clientId, action);
        return;
    }
    if(clientStates[clientId] == MenuState::ListRoomMembres){
        keyInputListMembres(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::AddMembre) {
        keyInputAddMembre(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::AddAdmin) {
        keyInputAddAdmin(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::RoomRequestList) {
        keyInputRequestList(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::ConfirmDeleteRoom) {
        keyInputConfirmDeleteRoom(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::ConfirmQuitRoom) {
        keyInputQuitRoom(clientSocket, clientId, action);
        return;
    }
    if(clientStates[clientId] == MenuState::Friends){
        keyInputFriendsMenu(clientSocket, clientId, action);
        return;
    }
    if(clientStates[clientId] == MenuState::AddFriend){
        keyInputAddFriendMenu(clientSocket, clientId, action);
        return;
    }
    if(clientStates[clientId] == MenuState::FriendList){
        keyInputManageFriendlist(clientSocket, clientId, action);
        return;
    }
    if(clientStates[clientId] == MenuState::FriendRequestList){
        keyInputManageFriendRequests(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::Game)
        clientStates[clientId] = MenuState::JoinOrCreateGame;
    if (clientStates[clientId] == MenuState::JoinOrCreateGame){
        keyInputJoinOrCreateGameMenu(clientSocket, clientId, action);
        return;
    }
        
    if (clientStates[clientId] == MenuState::JoinGame){
        keyInputChoiceGameRoom(clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::CreateGame){
        keyInputChooseGameModeMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::GameOver) {
        keyInputGameOverMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::Lobby) {
        //keyinputLobbyParametreMenu(clientSocket, clientId, action);
        return;
    }
    if (clientStates[clientId] == MenuState::Settings){
        //sendMenuToClient(clientSocket, menu.getLobbyMenu2(action));
        keyinputLobbyParametreMenu(clientSocket, clientId, action);
        return;
    }
}

/*void Server::keyinputLobbyParametreMenu(int clientSocket, int clientId, const std::string& action){
    this->SendInputLobby(clientId, action);
    sendMenuToClient(clientSocket, menu.getLobbyMenu2(getMaxPlayers(clientId), getMode(clientId), getAmountOfPlayers(clientId)));

    if (getAmountOfPlayers(clientId) == getMaxPlayers(clientId)){
        startGame(clientSocket, clientId);
    }
    

    //this->keyInputGameModeMenu(clientSocket, clientId, action);
}*/

/*void Server::startGame(int clientSocket, int clientId, std::shared_ptr<GameRoom> gameRoom){
    std::cout << "Démarrage du jeu pour le client #" << clientId << "." << std::endl;
    clientStates[clientId] = MenuState::Play;
    std::thread loopgame(&Server::loopGame, this, clientSocket, clientId, gameRoom);
    std::thread inputThread(&Server::receiveInputFromClient, this, clientSocket, clientId, gameRoom);
    
    loopgame.join();
    inputThread.join();

    deleteGameRoom(gameRoom->getRoomId());
    
    clientStates[clientId] = MenuState::GameOver;
    sendMenuToClient(clientSocket, menu.getGameOverMenu());

}*/

void Server:: keyInputChooseGameModeMenu(int clientSocket, int clientId, const std::string& action){
    if(action == "1"){
        //Endless
        std::cout << "Client #" << clientId << " a sélectionné Endless." << std::endl;
        //clientStates[clientId] = MenuState::Play;
        //sendMenuToClient(clientSocket, menu.getLobbyMenu());
        this->keyInputGameModeMenu(clientSocket, clientId);
        //int gameRoomId = clientGameRoomId[clientId];
        //startGame(clientSocket, clientId, gameRooms[gameRoomId]);

    }
    else if(action == "2"){
        //Duel
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu2(2, "Duel", 1));
        this->keyInputGameModeMenu(clientSocket, clientId, GameModeName::Duel);
        //int gameRoomId = clientGameRoomId[clientId];
        //startGame(clientSocket, clientId, gameRooms[gameRoomId]);
        
    }
    else if(action == "3"){
        //classic
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu1());
        this->keyInputGameModeMenu(clientSocket, clientId, GameModeName::Classic);
        //int gameRoomId = clientGameRoomId[clientId];
        //startGame(clientSocket, clientId, gameRooms[gameRoomId]);
    }
    else if(action == "4"){
        //royal competition
        clientStates[clientId] = MenuState::Settings;
        sendMenuToClient(clientSocket, menu.getLobbyMenu1());
        this->keyInputGameModeMenu(clientSocket, clientId, GameModeName::Royal_Competition);
        //int gameRoomId = clientGameRoomId[clientId];
        //startGame(clientSocket, clientId, gameRooms[gameRoomId]);
    }
    /*else if(action == "5"){
        clientStates[clientId] = MenuState::JoinOrCreateGame;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }*/
    else{
        sendMenuToClient(clientSocket, menu.getGameModeMenu());
    }
    //clientStates[clientId] = MenuState::CreateGame;
    //sendMenuToClient(clientSocket, menu.getGameModeMenu());
}


void Server::returnToMenu(int clientSocket, int clientId, MenuState state, const std::string& message, int sleepTime ) {
    if (!message.empty()) {
        sendMenuToClient(clientSocket, menu.displayMessage(message));
        sleep(sleepTime);
    }
    clientStates[clientId] = state;
    std::string currentUser = clientPseudo[clientId];
    std::string currentRoom = roomToManage[clientId];

    switch (state) {
        case MenuState::chat:
            sendMenuToClient(clientSocket, menu.getChatMenu());
            break;
        case MenuState::CreateRoom:
            sendMenuToClient(clientSocket, menu.getCreateChatRoomMenu());
            break;
        case MenuState::JoinRoom:
            sendMenuToClient(clientSocket, menu.getJoinChatRoomMenu());
            break;
        case MenuState::ManageRooms:
            sendMenuToClient(clientSocket, menu.getManageChatRoomsMenu(chat->getMyRooms(currentUser)));
            break;
        case MenuState::ManageRoom:
            sendMenuToClient(clientSocket, menu.getManageRoomMenu(chatRooms[currentRoom]->isAdmin(sockToPseudo[clientSocket]), chatRooms[currentRoom]->getadminPseudo().size() == 1));
            break;
        case MenuState::ListRoomMembres:
            sendMenuToClient(clientSocket, menu.getListeMembers(chatRooms[currentRoom]->getClients()));
            break;
        case MenuState::AddMembre:
            sendMenuToClient(clientSocket, menu.getAddMembreMenu());
            break;
        case MenuState::RoomRequestList:
            sendMenuToClient(clientSocket, menu.getListeRequests(chatRooms[currentRoom]->getReceivedReq()));
            break;
        case MenuState::FriendRequestList:
            sendMenuToClient(clientSocket, menu.getRequestsListMenu(friendList->getRequestList(currentUser)));
            break;
        case MenuState::FriendList:
            sendMenuToClient(clientSocket, menu.getFriendListMenu(friendList->getFriendList(currentUser)));
            break;
        case MenuState::AddFriend:
            sendMenuToClient(clientSocket, menu.getAddFriendMenu());
            break;
        case MenuState::Friends:
            sendMenuToClient(clientSocket, menu.getFriendMenu());
            break;
        case MenuState::ConfirmDeleteRoom:
            sendMenuToClient(clientSocket, menu.getDeleteRoomConfirmationMenu(currentRoom));
            break;
        case MenuState::ConfirmQuitRoom:
            sendMenuToClient(clientSocket, menu.getQuitRoomConfirmationMenu(currentRoom));


        default:
            break;
    }
}



void Server::keyInputManageFriendRequests(int clientSocket, int clientId, const std::string& action) {
    std::string currentUser = clientPseudo[clientId];

    if (action == "./ret") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
        return;
    }

    if (action.empty()) {
        return;
    }

    std::vector<std::string> requests = friendList->getRequestList(currentUser);
    if (requests.empty()) {
        return;
    }

    if (action == "accept.all" || action == "reject.all") {
        bool accept = (action == "accept.all");
        for (const std::string& friend_request : requests) {
            accept ? friendList->acceptFriendRequest(currentUser, friend_request)
                   : friendList->rejectFriendRequest(currentUser, friend_request);
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
        if (!friendList->isPendingRequest(friend_request, currentUser)) {
            returnToMenu(clientSocket, clientId, MenuState::Friends, "Erreur : Aucune demande d'ami en attente de '" + friend_request + "'.");
            return;
        }

        isAccept ? friendList->acceptFriendRequest(currentUser, friend_request)
                 : friendList->rejectFriendRequest(currentUser, friend_request);

        returnToMenu(clientSocket, clientId, MenuState::Friends,
            (isAccept ? "Demande d'ami acceptée avec " : "Demande d'ami rejetée pour '") + friend_request + "'.");
        return;
    }

    returnToMenu(clientSocket, clientId, MenuState::Friends, 
                 "Erreur : Format invalide. Utilisez 'accept.pseudo', 'accept.all', 'reject.pseudo' ou 'reject.all'.");
}


void Server::keyInputManageFriendlist(int clientSocket, int clientId, const std::string& action) {
    std::string currentUser = clientPseudo[clientId];

    if (action == "./ret") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
        return;
    }

    if (action.empty()) {
        return;
    }

    std::vector<std::string> friends = friendList->getFriendList(currentUser);
    if (friends.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::Main, "Vous n'avez aucun ami dans votre liste.");
        return;
    }

    if (action == "del.all") {
        for (const std::string& friend_name : friends) {
            friendList->removeFriend(currentUser, friend_name);
        }

        returnToMenu(clientSocket, clientId, MenuState::Friends, "Tous vos amis ont été supprimés.");
        return;
    }

    std::string prefix_del = "del.";
    if (action.rfind(prefix_del, 0) == 0) {  // Vérifie si action commence par "del."
        std::string friend_name = action.substr(prefix_del.size());

        if (!friendList->areFriends(currentUser, friend_name)) {
            returnToMenu(clientSocket, clientId, MenuState::Friends, "Erreur : '" + friend_name + "' n'est pas dans votre liste d'amis.");
            return;
        }

        friendList->removeFriend(currentUser, friend_name);
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
        returnToMenu(clientSocket, clientId, MenuState::Main);
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    }

}

void Server::keyInputAddFriendMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "./ret") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
        return;
    }
    if (action.empty()) {

        return;
    }
    std::string currentUser;
    {
        std::lock_guard<std::mutex> lock(clientPseudoMutex);
        currentUser = clientPseudo[clientId];
    }
    std::string friend_request = trim(action);

    // Vérifier si l'utilisateur existe
    if (!friendList->userExists(friend_request)) {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend, "Erreur : L'utilisateur " + friend_request + " n'existe pas.");
        return;
    }

    // Vérifier si l'utilisateur essaie de s'ajouter lui-même
    if (friend_request == currentUser) {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend, "Erreur : Vous ne pouvez pas vous ajouter vous-même.");
        return;
    }

    // Vérifier si les deux utilisateurs sont déjà amis
    if (friendList->areFriends(currentUser, friend_request)) {
        returnToMenu(clientSocket, clientId, MenuState::Friends, "Erreur : Vous êtes déjà ami avec " + friend_request + ".");
        return;
    }


    // Envoyer la demande d'ami
    friendList->sendFriendRequest(currentUser, friend_request);
    returnToMenu(clientSocket, clientId, MenuState::Friends, "Demande d'ami'Demande d'ami envoyée à " + friend_request + ". Veuiller consulter la listes des amis pour voir si la demande a été acceptée.");
    
}


void Server::keyInputGameModeMenu(int clientSocket, int clientId, GameModeName gameMode) {
    //création de la gameRoom (partie Endless pour l'instant)
    clientStates[clientId] = MenuState::Play;
    std::cout << "Création de la GameRoom pour le client #" << clientId << " avec le mode " << static_cast<int>(gameMode) << "." << std::endl;
    clientGameRoomId[clientId] = gameRoomIdCounter;
    std::shared_ptr<GameRoom> gameRoom = std::make_shared<GameRoom>(gameRoomIdCounter, clientId, gameMode);
    gameRooms.push_back(gameRoom);
    gameRoomIdCounter++;

    std::cout << "Démarrage du jeu pour le client #" << clientId << "." << std::endl;
    //clientStates[clientId] = MenuState::Play;
    std::thread loopgame(&Server::loopGame, this, clientSocket, clientId, gameRoom);
    std::thread inputThread(&Server::receiveInputFromClient, this, clientSocket, clientId, gameRoom);
    
    loopgame.join();
    inputThread.join();

    deleteGameRoom(gameRoom->getRoomId());
    
    clientStates[clientId] = MenuState::GameOver;
    sendMenuToClient(clientSocket, menu.getGameOverMenu());
}


std::string Server::trim(const std::string& s) {
    const char* whitespace = " \t\n\r";
    size_t start = s.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(whitespace);
    return s.substr(start, end - start + 1);
}

void Server::deleteGameRoom(int roomId) {
    gameRooms.erase(gameRooms.begin() + roomId);
    for (int idx=0; idx < clientIdCounter; idx++) {
        if (clientGameRoomId[idx] == roomId) {
            clientGameRoomId[idx] = -1;
        }
    }
    std::cout << "GameRoom #" << roomId << " deleted." << std::endl;


}

void Server::shiftGameRooms(int index) {
    int size = gameRooms.size();
    for (int i = index; i < size - 1; ++i) {
        gameRooms[i] = gameRooms[i + 1];
        gameRooms[i]->setRoomId(i);
    }   
    gameRooms.pop_back();
}

void Server::sendInputToGameRoom(int clientId, const std::string& action, std::shared_ptr<GameRoom> gameRoom) {
    gameRoom->input(clientId, action);
}

//recuperer les inputs du client
void Server::receiveInputFromClient(int clientSocket, int clientId, std::shared_ptr<GameRoom> gameRoom) {
    char buffer[1024];
    while ((clientStates[clientId] != MenuState::Play) || (!gameRoom->getHasStarted())){
        continue;
    }
    std::cout << "reception des entrées du client " << clientId << std::endl;
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); 
        if (bytesReceived > 0) {
            try {
                json receivedData = json::parse(buffer);
                std::string action = receivedData[jsonKeys::ACTION];
                
                std::cout << "Action reçue du client " << clientId << " : " << action << std::endl;
                if ((clientStates[clientId] == MenuState::Play) &&
                (gameRoom.get() != nullptr) && (!gameRoom->getGameIsOver(clientId))
                && gameRoom->getInProgress())
                    sendInputToGameRoom(clientId, action, gameRoom);
                else 
                    break;
            }
            catch (json::parse_error& e) {
                std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
            }
        }
    }
    std::cout << "fini les entrées" << std::endl;
}

void Server::loopGame(int clientSocket, int clientId, std::shared_ptr<GameRoom> gameRoom) {
    int gameRoomId = gameRoom->getRoomId();
    std::thread gameRoomThread(&GameRoom::startGame, gameRoom);
    while (!gameRoom->getHasStarted())
        continue;
    std::shared_ptr<Score> score = (gameRoom->getGameModeName() == GameModeName::Endless) ? gameRoom->getScore(clientId) : nullptr;
    std::shared_ptr<Game> game = gameRoom->getGame(clientId);
    std::cout << "Game #" << gameRoomId << " started." << std::endl;
    std::cout<<"getInProgress : "<<gameRoom->getInProgress()<<std::endl;
    while (gameRoom->getInProgress()) { 
        std::cout<<"bb"<<std::endl;
        if ((gameRoom->getGameIsOver(clientId))){
            std::cout<<"cc"<<std::endl;
            if (gameRoom->getGameModeName() == GameModeName::Endless)
                break;
            else if ((gameRoom->getAmountOfPlayers() < 2))
                break;
        }
        if (gameRoom->getNeedToSendGame(clientId)) { 
            std::cout<<"salamo 3alaykom"<<std::endl;
            sendGameToPlayer(clientSocket, game);
            gameRoom->setNeedToSendGame(false, clientId);
        }

    }
    gameRoomThread.join();
    if (score != nullptr)
        userManager->updateHighscore(clientPseudo[clientId], score->getScore());
    std::cout << "Game #" << gameRoomId << " ended." << std::endl;

}

/*void Server::sendGameToClient(int clientSocket, int clientId) { //TODO: Deplacer le main pour faire un fichier game ??? 
    int gameRoomId = clientGameRoomId[clientId];
    std::shared_ptr<Game> gameRoom = gameRooms[gameRoomId]->getGame(clientId); // Récupérer la partie du client

    json message;
    
    message["score"] = gameRoom->getScore().scoreToJson();
    message["grid"] = gameRoom->getGrid().gridToJson();
    message["tetraPiece"] = gameRoom->getCurrentPiece().tetraminoToJson(); // Ajout du tétrimino dans le même message

    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0); // Un seul envoi
}*/



void Server::keyInputWelcomeMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {  // Se connecter

        clientStates[clientId] = MenuState::LoginPseudo;
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
        close(clientSocket);
        //return to terminal
        return;
    }
    else {
        sendMenuToClient(clientSocket, menu.getMainMenu0());
    }
}

void Server::keyInputRegisterPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    if (userManager->userNotExists(action)) { 
        // Si le pseudo n'existe pas, on stock en tmp
        clientPseudo[clientId] = action;
        clientStates[clientId] = MenuState::RegisterPassword;
        sendMenuToClient(clientSocket, menu.getRegisterMenu2());
    } 
    else {
        sendMenuToClient(clientSocket, menu.getRegisterMenu1());
    }
}

void Server::keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    friendList->registerUser(clientPseudo[clientId]);
    userManager->registerUser(clientPseudo[clientId], action);
    chat->initMessageMemory("Clients/" + clientPseudo[clientId] + ".json");
    sockToPseudo[clientSocket] = clientPseudo[clientId];
    pseudoTosocket[clientPseudo[clientId]] = clientSocket;
    clientStates[clientId] = MenuState::Main;
    sendMenuToClient(clientSocket, menu.getMainMenu1());
}


void Server::keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    std::cout << "Pseudo: " << action << std::endl;
    if (!userManager->userNotExists(action)) { // Si le pseudo existe
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
    if (userManager->authenticateUser(clientPseudo[clientId], action)) { // Si le mot de passe est correct
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
    if (action == "1") { // créer une gameRoom (choisir s'il veut créer sa partie ou rejoindre une partie)
        
        clientStates[clientId] = MenuState::Game;
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }
    else if (action == "2") {
        clientStates[clientId] = MenuState::Friends;
        sendMenuToClient(clientSocket, menu.getFriendMenu());
    }
    else if (action == "3") {
        // Classement
        clientStates[clientId] = MenuState::classement;
        // TODO: en gros c'est brouillon pour le moment parce que faudrait pas faire passser menu par
        // la game mais avoir une instance de menu dans le serveur comme ici
        sendMenuToClient(clientSocket, menu.getRankingMenu(userManager->getRanking())); 
  
    }
    else if (action == "4") {
        // Chat
        clientStates[clientId] = MenuState::chat;
        sendMenuToClient(clientSocket, menu.getChatMenu());
        
    }
    
    else if (action == "5") {
        // Retour à l'écran précédent
        clientStates[clientId] = MenuState::Welcome;
        sendMenuToClient(clientSocket, menu.getMainMenu0());
        
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
        std::vector<std::vector<std::string>> invitations = friendList->getListGameRequest(currentUser);
        sendMenuToClient(clientSocket, menu.getGameRequestsListMenu(invitations));
        // Rejoindre une partie
    }
    else if (action == "3") {
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, menu.getMainMenu1());
        
    }
    else {
        sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
    }
}


//Définir les actions possibles dans le chat


void Server::keyInputChatMenu(int clientSocket, int clientId, const std::string& action) {
    if(action == "1") {
        clientStates[clientId] = MenuState::CreateRoom;
        sendMenuToClient(clientSocket, menu.getCreateChatRoomMenu());
        
    }
    else if(action == "2") {
        clientStates[clientId] = MenuState::JoinRoom;
        sendMenuToClient(clientSocket, menu.getJoinChatRoomMenu());
        
    }
    else if(action == "3") {
        clientStates[clientId] = MenuState::ManageRooms;
        sendMenuToClient(clientSocket, menu.getManageChatRoomsMenu(chat->getMyRooms(sockToPseudo[clientSocket])));
    }
    else if(action == "4") {
        sendChatModeToClient(clientSocket);
        setRunningChat(clientSocket, true);
        chat->processClientChat(clientSocket, clientId, *this, MenuState::chat, menu.getChatMenu());
    }
    else if(action == "5") {
        returnToMenu(clientSocket, clientId, MenuState::Main);
        
    }
}


void Server::keyInputCreateChatRoom(int clientSocket, int clientId, const std::string& action) {
    if(action == "./quit") {
        returnToMenu(clientSocket, clientId, MenuState::chat);
    }
    else if(action.empty()) {
        sendMenuToClient(clientSocket, menu.getCreateChatRoomMenu());
    }
    else if(chatRooms.find(action) != chatRooms.end()){
        returnToMenu(clientSocket, clientId, MenuState::chat, "Room name already exists.");
    }
    else{
        chatRooms[action] = std::make_shared<chatRoom>(action, sockToPseudo[clientSocket]);
        returnToMenu(clientSocket, clientId, MenuState::chat, "Room created successfully.");
    }

}

void Server::keyInputJoinChatRoom(int clientSocket, int clientId, const std::string& action) {
    if(action == "./quit") {
        returnToMenu(clientSocket, clientId, MenuState::chat);
    }
    else if(action.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::chat);
    }
    else if(chatRooms.find(action) == chatRooms.end()){
        returnToMenu(clientSocket, clientId, MenuState::chat, "Room name does not exist.");
    }
    else if(chatRooms[action]->isClient(sockToPseudo[clientSocket])){
        returnToMenu(clientSocket, clientId, MenuState::chat, "You are already a member in this room");
    }
    else if(chatRooms[action]->isInReceivedReq(sockToPseudo[clientSocket])){
        returnToMenu(clientSocket, clientId, MenuState::chat, "You already sent a request to join this room");
    }
    else{
        chatRooms[action]->addReceivedRequest(sockToPseudo[clientSocket]);
        returnToMenu(clientSocket, clientId, MenuState::chat, "Request to join room sent succesfully.");
    }
}

void Server::keyInputManageMyRooms(int clientSocket, int clientId, const std::string& action) {
    std::vector<std::string> myRooms = chat->getMyRooms(sockToPseudo[clientSocket]);
    if(action == "./quit") {
        returnToMenu(clientSocket, clientId, MenuState::chat);
    }
    else if(action.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::ManageRooms);
    } 
    else if(std::find(myRooms.begin(), myRooms.end(), action) == myRooms.end()) {
        returnToMenu(clientSocket, clientId, MenuState::chat, "Room name does not exist.");
    } 
    else {
        roomToManage[clientId] = action;
        clientStates[clientId] = MenuState::ManageRoom;
        sendMenuToClient(clientSocket, menu.getManageRoomMenu(chatRooms[action]->isAdmin(sockToPseudo[clientSocket]), chatRooms[action]->getadminPseudo().size() == 1));
    }
}

void Server::keyInputManageRoom(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "./quit") {
        returnToMenu(clientSocket, clientId, MenuState::ManageRooms);
    }
    else if(action == "1") {
        clientStates[clientId] = MenuState::ListRoomMembres;
        sendMenuToClient(clientSocket, menu.getListeMembers(chatRooms[roomName]->getClients()));
    }
    bool isAdmin = chatRooms[roomName]->isAdmin(sockToPseudo[clientSocket]);
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
            sendMenuToClient(clientSocket, menu.getListeRequests(chatRooms[roomName]->getReceivedReq()));
        }
        //Supression de la room
        else if(action == "5") {
            if (chatRooms[roomName]->getClients().size() == 1) {
                returnToMenu(clientSocket, clientId, MenuState::ConfirmDeleteRoom);

            } else {
                returnToMenu(clientSocket, clientId, MenuState::ConfirmQuitRoom );
            }
        }else if(action == "6") {
            //retour
            returnToMenu(clientSocket, clientId, MenuState::ManageRooms);
        }
    }
    else {
        if(action == "2") {
            returnToMenu(clientSocket, clientId, MenuState::ConfirmQuitRoom );
        }else if(action == "3") {
            //retour
            returnToMenu(clientSocket, clientId, MenuState::ManageRooms);
        }
    }
}
void Server::keyInputConfirmDeleteRoom(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if (action == "oui") {
        chatRooms[roomName]->deleteRoomFile();
        chatRooms.erase(roomName);
        returnToMenu(clientSocket, clientId, MenuState::ManageRooms, "Room '" + roomName + "' supprimée avec succès.");
    } else {
        returnToMenu(clientSocket, clientId, MenuState::ManageRooms, "Suppression de la room annulée.");
    }
}


void Server::keyInputListMembres(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "./quit") {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }
    else if(action.rfind("del.", 0) == 0){
        std::string pseudo = action.substr(4);
        if(chatRooms[roomName]->isClient(pseudo)){
            if(chatRooms[roomName]->isAdmin(pseudo)){
                returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "You cannot remove an admin from the room.");
            }
            else{
                chatRooms[roomName]->removeClient(pseudo);
                returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User removed successfully.");
            }
        }
    }
    else {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }

}

void Server::keyInputAddMembre(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "./quit") {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }
    else if(action.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::AddMembre);
    }
    else if(chatRooms[roomName]->isClient(action)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User is already in the room.");
    }
    else if(!userManager->userNotExists(action)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User does not exist.");
    }
    else {
        //a gerer envoyer une demande d'ajout au client
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "Request sent successfully.");
    }
}

void Server::keyInputAddAdmin(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "./quit") {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }
    else if(action.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::AddAdmin);
    }
    else if(chatRooms[roomName]->isAdmin(action)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User is already an admin.");
    }
    else if(!chatRooms[roomName]->isClient(action)) {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User is not in the room.");
    }
    else {
        chatRooms[roomName]->addAdmin(action);
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User is now an admin.");
    }
}

void Server::keyInputRequestList(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "./quit") {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }
    else if(action.rfind("accept.", 0) == 0){
        std::string pseudo = action.substr(7);
        if(chatRooms[roomName]->isClient(pseudo)){
            returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User is already in the room.");
        }
        else{
            chatRooms[roomName]->acceptClientRequest(pseudo);
            returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User added successfully.");
        }
    }
    else if(action.rfind("reject.", 0) == 0){
        std::string pseudo = action.substr(7);
        if(chatRooms[roomName]->isClient(pseudo)){
            returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User is already in the room.");
        }
        else{
            chatRooms[roomName]->refuseClientRequest(pseudo);
            returnToMenu(clientSocket, clientId, MenuState::ManageRoom, "User request rejected.");
        }
    }
    else {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }
}

void Server::keyInputQuitRoom(int clientSocket, int clientId, const std::string& action){
    std::string roomName = roomToManage[clientId];
    std::string userPseudo = sockToPseudo[clientSocket];

    if(action == "./quit") {
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
        return;
    }

    if (chatRooms.find(roomName) == chatRooms.end()) {
        returnToMenu(clientSocket, clientId, MenuState::ManageRooms, "Room does not exist.");
        return;
    }

    auto room = chatRooms[roomName];
    bool isAdmin = room->isAdmin(userPseudo);
    if (action == "oui"){
        if (isAdmin) {
            if (room->getadminPseudo().size() > 1) {
                room->removeAdmin(userPseudo);
                room->removeClient(userPseudo);
                returnToMenu(clientSocket, clientId, MenuState::ManageRooms, "You have left the room.");
            } else {
                room->deleteRoomFile();
                chatRooms.erase(roomName);
                returnToMenu(clientSocket, clientId, MenuState::ManageRooms, "Room deleted successfully.");
            }
        } else {
            room->removeClient(userPseudo);
            returnToMenu(clientSocket, clientId, MenuState::ManageRooms, "You have left the room.");
        }        
    }else{
        //retour au menu de gestion de la room (si on a pas quitté/ supprimé la room)
        returnToMenu(clientSocket, clientId, MenuState::ManageRoom);
    }

}



void Server::loadChatRooms() {
    // Lister tous les fichiers dans le dossier ChatRooms
    std::filesystem::path path = "ChatRooms/";
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string filename = entry.path().filename().string();
        std::cout << "Loading chat room: " << filename << std::endl;
    }
}


//Définir les actions possibles dans le classement

void Server::keyInputRankingMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") { // TODO: faudra qu'on le voit dans le menu
        clientStates[clientId] = MenuState::Main;
        sendMenuToClient(clientSocket, menu.getMainMenu1());
    }
    else {
        sendMenuToClient(clientSocket, menu.getRankingMenu(userManager->getRanking())); 
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

void Server::sendGameToPlayer(int clientSocket, std::shared_ptr<Game> game) {
    json message;
    
    message[jsonKeys::SCORE] = game->getScore()->scoreToJson();
    message[jsonKeys::GRID] = game->getGrid()->gridToJson();
    message[jsonKeys::TETRA_PIECE] = game->getCurrentPiece().tetraminoToJson(); // Ajout du tétrimino dans le même message

    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0); // Un seul envoi
}

//recuperer les inputs du client
/*void Server::receiveInputFromClient(int clientSocket, int clientId, std::shared_ptr<GameRoom> gameRoom) {
    char buffer[1024];
    while ((clientStates[clientId] != MenuState::Play) || (!gameRoom->getHasStarted())){
        //std::cout << "yalahwi" << std::endl;
        //keyinputLobbyParametreMenu(clientSocket, clientId, action);
        continue;
    }
    std::cout << "reception des entrées du client " << clientId << std::endl;
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); 
        if (bytesReceived > 0) {
            try {
                json receivedData = json::parse(buffer);
                std::string action = receivedData["action"];
                
                std::cout << "Action reçue du client " << clientId << " : " << action << std::endl;
                if ((clientStates[clientId] == MenuState::Play) &&
                (gameRoom.get() != nullptr) && (!gameRoom->getGameIsOver(clientId))
                && gameRoom->getInProgress())
                    sendInputToGameRoom(clientId, action, gameRoom);
                else 
                    break;
            }
            catch (json::parse_error& e) {
                std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
            }
        }
    }
    std::cout << "fini les entrées" << std::endl;

}*/



void Server::sendChatModeToClient(int clientSocket) {
    json message;
    message[jsonKeys::MODE] = "chat";
    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0);
}


bool Server::getRunningChat(int clientId) {
    auto it = runningChats.find(clientId);
    if (it != runningChats.end()) {
        return it->second;
    }
    return false;
}

void Server::setRunningChat(int clientId, bool value) {
    runningChats[clientId] = value;
}

void Server::setClientState(int clientId, MenuState state) {
    clientStates[clientId] = state;
}


////////////////Ce qui suit est pour les invitations de jeu/////////////////
//************************************************************************ */


void Server::keyInputSendGameRequestMenu(int clientSocket, int clientId, std::string receiver, std::string status) {
    /*if (action.empty()) {
        return;
    }*/
    //int nombreJoueursAcceptes = 0;

    std::cout << "Sending game request to: " << receiver << " with status: " << status << std::endl;

    std::string game_request = receiver;
    std::string currentUser = clientPseudo[clientId];
    //std::vector<std::string> invitedplyers;
    //std::vector<std::string> invitedObservers;
    if(game_request == currentUser){
        sendMenuToClient(clientSocket, menu.displayMessage("Erreur : Vous ne pouvez pas vous envoyer une invitation de jeu."));
        sleep(3);
        //clientStates[clientId] = MenuState::Main;
        //sendMenuToClient(clientSocket, menu.getMainMenu1());
        return;
    }
    if(!friendList->areFriends(currentUser,game_request)){
        sendMenuToClient(clientSocket, menu.displayMessage("Erreur : Vous n'êtes pas ami avec "+game_request+"."));
        sleep(3);
        //clientStates[clientId] = MenuState::Main;
        //sendMenuToClient(clientSocket, menu.getMainMenu1());
        return;
    }
    int gameRoomId = clientGameRoomId[clientId];
    friendList->sendInvitationToFriend(currentUser, game_request,status, gameRoomId);
    sendMenuToClient(clientSocket, menu.displayMessage("Request Sent To "+game_request+"."));
    sleep(3);
    /* si le nombre n'est pas atteint on peut continuer à inviter des joueurs dons afficher ce menu 
    clientStates[clientId] = MenuState::Main;
    */
    //sendMenuToClient(clientSocket, menu.getMainMenu1());
    //sendMenuToFriend(currentUser, game_request);

    //clientStates[clientId] = MenuState::Settings;
    //sendMenuToClient(clientSocket, menu.getLobbyMenu2(getMaxPlayers(clientId), getMode(clientId), getAmountOfPlayers(clientId)));


}



void Server::keyinputLobbyParametreMenu(int clientSocket, int clientId, const std::string& action) {

    std::cout << "Received action: " << action << " from client #" << clientId << std::endl;

    // Vérifier si l'action commence par \invite
    if (action.find("\\invite") == 0) {
        // Extraire le statut et le receiver de l'action
        size_t firstBackSlash = action.find('\\', std::string("\\invite").size());  // Trouver le premier \ après \invite
        if (firstBackSlash == std::string::npos) {
            // Si le format est incorrect, retourner sans faire rien
            return;
        }

        size_t secondBackSlash = action.find('\\', firstBackSlash + 1);  // Trouver le deuxième \ après le statut
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
        //return;
    }

    // Continuer la logique existante pour la gestion du lobby
    this->SendInputLobby(clientId, action);    
    sendMenuToClient(clientSocket, menu.getLobbyMenu2(getMaxPlayers(clientId), getMode(clientId), getAmountOfPlayers(clientId)));
    //int gameRoomId = clientGameRoomId[clientId];
    //startGame(clientSocket, clientId, gameRooms[gameRoomId]);

    
}


void Server::keyInputChoiceGameRoom(int clientId, const std::string& action){
    if (action.find("accept.") == 0){
        size_t pos = action.find(".");
        std::string number = action.substr(pos+1, action.size());
        int roomNumber = std::stoi(number);
        clientGameRoomId[clientId] = roomNumber;

        gameRooms[roomNumber]->addPlayer(clientId);
        clientStates[clientId] = MenuState::Settings;

        //sendMenuToClient(clientSocket, menu.getLobbyMenu2(getMaxPlayers(clientId), getMode(clientId), getAmountOfPlayers(clientId)));
        for (const auto& cId : gameRooms[roomNumber]->getPlayers()){
        
            sendMenuToClient(pseudoTosocket[clientPseudo[cId]], menu.getLobbyMenu2(getMaxPlayers(cId), getMode(cId), getAmountOfPlayers(cId)));
            
        }
        //startGame(pseudoTosocket[clientPseudo[clientId]], clientId, gameRooms[roomNumber]);

        /*if (getAmountOfPlayers(clientId) == getMaxPlayers(clientId)){
            for(const auto& cId : gameRooms[roomNumber]->getPlayers()){
                startGame(pseudoTosocket[clientPseudo[cId]], cId, gameRooms[roomNumber]);
                //clientStates[cId] = MenuState::Play;
            }

            //startGame(pseudoTosocket[clientPseudo[clientId]], clientId, gameRooms[roomNumber]);
            
        }*/
    }
    

    

}


void Server::SendInputLobby(int clientId, const std::string& action){
    int gameRoomId = clientGameRoomId[clientId];
    gameRooms[gameRoomId]->inputLobby(action);
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

        // Create a directory to store client messages if it doesn't exist
        std::filesystem::create_directories("Clients");
        // Create a directort to store chatRooms data if it doesn't exist
        std::filesystem::create_directories("ChatRooms");


        Server server(12345);
        if (!server.start()) {
            std::cerr << "Erreur: Impossible de démarrer le serveur." << std::endl;
            return 1;
        }
        server.loadChatRooms();

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


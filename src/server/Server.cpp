#include "Server.hpp"
#include "../common/json.hpp"
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

            if (!receivedData.contains("action") || !receivedData["action"].is_string()) {
                std::cerr << "Erreur: 'action' manquant ou invalide dans le JSON reçu." << std::endl;
                return;
            }

            std::string action = receivedData["action"];
            handleMenu(clientSocket, clientId, action); // Gérer l'action du client
        } catch (json::parse_error& e) {
            std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
        }
    }
}


void Server::handleMenu(int clientSocket, int clientId, const std::string& action) {
    MenuState state = clientStates[clientId];
    switch (state) {
        case MenuState::Welcome:
            keyInputWelcomeMenu(clientSocket, clientId, action);
            break;
        case MenuState::RegisterPseudo:
            keyInputRegisterPseudoMenu(clientSocket, clientId, action);
            break;
        case MenuState::RegisterPassword:
            keyInputRegisterPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::LoginPseudo:
            keyInputLoginPseudoMenu(clientSocket, clientId, action);
            break;
        case MenuState::LoginPassword:
            std::cout << "LoginPassword" << std::endl;
            keyInputLoginPasswordMenu(clientSocket, clientId, action);
            break;
        case MenuState::Main:
            keyInputMainMenu(clientSocket, clientId, action);
            break;
        case MenuState::classement:
            keyInputRankingMenu(clientSocket, clientId, action);
            break;
        case MenuState::chat:
            keyInputChatMenu(clientSocket, clientId, action);
            break;
        case MenuState::CreateRoom:
            keyInputCreateChatRoom(clientSocket, clientId, action);
            break;
        case MenuState::JoinRoom:
            keyInputJoinChatRoom(clientSocket, clientId, action);
            break;
        case MenuState::ManageRooms:
            keyInputManageMyRooms(clientSocket, clientId, action);
            break;
        case MenuState::ManageRoom:
            //sendMenuToClient(clientSocket, menu.getManageRoomMenu(currentUser == chatRooms[clientGameRoomId[clientId]]->getAdmin()));
            break;
        case MenuState::AddMembre:
            sendMenuToClient(clientSocket, menu.getAddMembreMenu());
            break;
        case MenuState::RemoveMembre:
            sendMenuToClient(clientSocket, menu.getRemoveMembreMenu());
            break;
        case MenuState::Friends:
            keyInputFriendsMenu(clientSocket, clientId, action);
            break;
        case MenuState::AddFriend:
            keyInputAddFriendMenu(clientSocket, clientId, action);
            break;
        case MenuState::FriendList:
            keyInputManageFriendlist(clientSocket, clientId,action);
            break;
        case MenuState::FriendRequestList:
            keyInputManageFriendRequests(clientSocket, clientId,action);
            break;
        case MenuState::JoinGame:
            keyInputGameModeMenu(clientSocket, clientId);
            break;
        case MenuState::CreateGame:
            keyInputGameModeMenu(clientSocket, clientId);
            break;
        case MenuState::Play: 
            sendInputToGameRoom(clientId, action);          
            break;
        case MenuState::Game:
            clientStates[clientId] = MenuState::JoinOrCreateGame;
            [[fallthrough]];
        case MenuState::JoinOrCreateGame:
            keyInputJoinOrCreateGameMenu(clientSocket, clientId, action);
            break;
        case MenuState::GameOver:
            keyInputGameOverMenu(clientSocket, clientId, action);
            break;
        default:
            break;
    }


}

void Server::returnToMenu(int clientSocket, int clientId, MenuState state, const std::string& message, int sleepTime ) {
    if (!message.empty()) {
        sendMenuToClient(clientSocket, menu.displayMessage(message));
        sleep(sleepTime);
    }
    clientStates[clientId] = state;
    std::string currentUser = clientPseudo[clientId];

    switch (state) {
        case MenuState::Game:
            sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
            break;
        case MenuState::JoinOrCreateGame:
            sendMenuToClient(clientSocket, menu.getJoinOrCreateGame());
            break;
        case MenuState::classement:
            sendMenuToClient(clientSocket, menu.getRankingMenu(userManager->getRanking()));
            break;
        case MenuState::chat:
            sendMenuToClient(clientSocket, menu.getChatMenu());
            break;
        case MenuState::CreateRoom:
            sendMenuToClient(clientSocket, menu.getCreateChatRoomMenu());
            break;
        case MenuState::JoinRoom:
            sendMenuToClient(clientSocket, menu.getJoinChatRoomMenu());
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
        case MenuState::LoginPseudo:
            sendMenuToClient(clientSocket, menu.getLoginMenu1());
            break;
        case MenuState::RegisterPseudo:
            sendMenuToClient(clientSocket, menu.getRegisterMenu1());
            break;
        case MenuState::RegisterPassword:
            sendMenuToClient(clientSocket, menu.getRegisterMenu2());
            break;
        case MenuState::LoginPassword:
            sendMenuToClient(clientSocket, menu.getLoginMenu2());
            break;    
        case MenuState::Welcome:
            sendMenuToClient(clientSocket, menu.getMainMenu0());
            break;
        case MenuState::Main:
            sendMenuToClient(clientSocket, menu.getMainMenu1());
            break;
        
        default:
            break;
    }
}


void Server::keyInputManageFriendRequests(int clientSocket, int clientId, const std::string& action) {
    std::string currentUser = clientPseudo[clientId];

    if (action == "./ret") {
        returnToMenu(clientSocket, clientId, MenuState::Main);
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
        returnToMenu(clientSocket, clientId, MenuState::Main);
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
        
    }

}

void Server::keyInputAddFriendMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "./ret") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
        //clientStates[clientId] = MenuState::Main;
        //sendMenuToClient(clientSocket, menu.getMainMenu1());
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

    //std::string friend_request = action; 


    //std::string currentUser = clientPseudo[clientId]; 


    std::string friend_request = trim(action);
    //currentUser = trim(clientPseudo[clientId]);
    /*if (friend_request.empty() && currentUser.empty()) {
        returnToMenu(clientSocket, clientId, MenuState::AddFriend, "Erreur : Probleme de client ID ou de pseudo.");
        return;
    }*/
    std::cout << "Pseudo actuel: '" << currentUser << "'" << std::endl;
    std::cout << "Demande d'ami pour: '" << friend_request << "'" << std::endl;

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
    clientGameRoomId[clientId] = gameRoomIdCounter;
    gameRooms.push_back(std::make_shared<GameRoom>(gameRoomIdCounter, clientId, gameMode));
    gameRoomIdCounter++;
    std::thread toSendGameToClient(&Server::loopGame, this, clientSocket, clientId);
    toSendGameToClient.detach();
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
    gameRoomIdCounter--;
    if ((roomId < gameRoomIdCounter) && (gameRoomIdCounter > 1))
        this->shiftGameRooms(roomId);
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

void Server::sendInputToGameRoom(int clientId, const std::string& action) {
    int roomId = clientGameRoomId[clientId];
    gameRooms[roomId]->keyInputGame(clientId, action);
}

void Server::loopGame(int clientSocket, int clientId) {
    int gameRoomId = clientGameRoomId[clientId];
    while (!gameRooms[gameRoomId]->getHasStarted())
        continue;
    std::cout << "Game #" << gameRoomId << " started." << std::endl;
    while (gameRooms[gameRoomId]->getInProgress()) { 
        if ((gameRooms[gameRoomId]->getGameIsOver(clientId))){
            if (gameRooms[gameRoomId]->getGameModeName() == GameModeName::Endless)
                break;
            else if ((gameRooms[gameRoomId]->getAmountOfPlayers() < 2))
                break;
        }
        if (gameRooms[gameRoomId]->getNeedToSendGame(clientId)) { 
            sendGameToClient(clientSocket, clientId);
            gameRooms[gameRoomId]->setNeedToSendGame(false, clientId);
        }
    }
    clientStates[clientId] = MenuState::GameOver;
    if (gameRooms[gameRoomId]->getGameModeName() == GameModeName::Endless)
        userManager->updateHighscore(clientPseudo[clientId], gameRooms[gameRoomId]->getScore(clientId).getScore());
    deleteGameRoom(gameRoomId);
    sendMenuToClient(clientSocket, menu.getGameOverMenu());
    std::cout << "Game #" << gameRoomId << " ended." << std::endl;
}

void Server::sendGameToClient(int clientSocket, int clientId) { //TODO: Deplacer le main pour faire un fichier game ??? 
    int gameRoomId = clientGameRoomId[clientId];
    std::shared_ptr<Game> gameRoom = gameRooms[gameRoomId]->getGame(clientId); // Récupérer la partie du client

    json message;
    
    message["score"] = gameRoom->getScore().scoreToJson();
    message["grid"] = gameRoom->getGrid().gridToJson();
    message["tetraPiece"] = gameRoom->getCurrentPiece().tetraminoToJson(); // Ajout du tétrimino dans le même message

    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0); // Un seul envoi
}



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
    if (action == "./ret") {  // Valider le pseudo
        returnToMenu(clientSocket, clientId, MenuState::Welcome);
        return;
    }
    if (userManager->userNotExists(action)) { 
        {
            std::lock_guard<std::mutex> lock(clientPseudoMutex);
            clientPseudo[clientId] = action;
        }
        clientStates[clientId] = MenuState::RegisterPassword;
        sendMenuToClient(clientSocket, menu.getRegisterMenu2());
        
    } 
    else {
        sendMenuToClient(clientSocket, menu.getRegisterMenu1());
    }
}

void Server::keyInputRegisterPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "./ret") {  
        returnToMenu(clientSocket, clientId, MenuState::Welcome);
        return;
    }
    friendList->registerUser(clientPseudo[clientId]);
    userManager->registerUser(clientPseudo[clientId], action);
    chat->initMessageMemory("Clients/" + clientPseudo[clientId] + ".json");

    sockToPseudo[clientSocket] = clientPseudo[clientId];
    pseudoTosocket[clientPseudo[clientId]] = clientSocket;
    //clientPseudo.erase(clientId);
    returnToMenu(clientSocket, clientId, MenuState::Main);
    
    
}

void Server::keyInputLoginPseudoMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "./ret") { 
        returnToMenu(clientSocket, clientId, MenuState::Welcome);
        return;
    }
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
        clientStates[clientId] = MenuState::LoginPseudo;
        sendMenuToClient(clientSocket, menu.getLoginMenuFailed1());
    }
}

void Server::keyInputLoginPasswordMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "./ret") {  
        returnToMenu(clientSocket, clientId, MenuState::Welcome);
        return;
    }
    if (userManager->authenticateUser(clientPseudo[clientId], action)) { // Si le mot de passe est correct
        
        
        pseudoTosocket[clientPseudo[clientId]] = clientSocket;
        sockToPseudo[clientSocket] = clientPseudo[clientId];
        chat->initMessageMemory("Clients/" + clientPseudo[clientId] + ".json");
        returnToMenu(clientSocket, clientId, MenuState::Main);
        
        

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
    if (action == "2") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
       

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
        clientStates[clientId] = MenuState::CreateGame;
        // Raccourci vers une game Endless car on doit implémenter le reste
        }
    else if (action == "2") {
        clientStates[clientId] = MenuState::JoinGame;
        // Rejoindre une partie
    }
    else if (action == "3") {
        returnToMenu(clientSocket, clientId, MenuState::Main);
        
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
    }else if(action.empty()) {
        sendMenuToClient(clientSocket, menu.getCreateChatRoomMenu());
    }else if(nameChatRoomIndex.find(action) != nameChatRoomIndex.end()){
        returnToMenu(clientSocket, clientId, MenuState::chat, "Room name already exists.", 5);
    }else{
        nameChatRoomIndex[action] = chatRoomIdCounter++;
        chatRooms.push_back(std::make_shared<chatRoom>(action, sockToPseudo[clientSocket]));
        returnToMenu(clientSocket, clientId, MenuState::chat, "Room created successfully.");
    }

}

void Server::keyInputJoinChatRoom(int clientSocket, int clientId, const std::string& action) {
    if(action == "./quit") {
        returnToMenu(clientSocket, clientId, MenuState::chat);
    }
    else if(action.empty()) {
        return;
    }else if(nameChatRoomIndex.find(action) == nameChatRoomIndex.end()){
        returnToMenu(clientSocket, clientId, MenuState::chat, "Room name does not exist.", 5);
    }else if(chatRooms[nameChatRoomIndex[action]]->isClient(sockToPseudo[clientSocket])){
        returnToMenu(clientSocket, clientId, MenuState::chat, "You are already a member in this room", 5);
    }
    else{
        chatRooms[nameChatRoomIndex[action]]->addReceivedRequest(sockToPseudo[clientSocket]);
        returnToMenu(clientSocket, clientId, MenuState::chat, "Request to join room sent succesfully.");
    }
}

void Server::keyInputManageMyRooms(int clientSocket, int clientId, const std::string& action) {
    std::vector<std::string> myRooms = chat->getMyRooms(sockToPseudo[clientSocket]);
    if(action == "./quit") {
        returnToMenu(clientSocket, clientId, MenuState::chat);
    }
    else if(action.empty()) {
        return;
    } else if(std::find(myRooms.begin(), myRooms.end(), action) == myRooms.end()) {
        returnToMenu(clientSocket, clientId, MenuState::chat, "Room name does not exist.", 5);
    } else {
        clientStates[clientId] = MenuState::ManageRoom;
        sendMenuToClient(clientSocket, menu.getManageRoomMenu(chatRooms[nameChatRoomIndex[action]]->isAdmin(sockToPseudo[clientSocket])));
    }
}



//Définir les actions possibles dans le classement

void Server::keyInputRankingMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") { // TODO: faudra qu'on le voit dans le menu
        returnToMenu(clientSocket, clientId, MenuState::Main);

    }
    else {
        sendMenuToClient(clientSocket, menu.getRankingMenu(userManager->getRanking())); 
    }
}

void Server::keyInputGameOverMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "1") {
        returnToMenu(clientSocket, clientId, MenuState::JoinOrCreateGame);

    }
    else if (action == "2") {
        returnToMenu(clientSocket, clientId, MenuState::Main);

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

void Server::sendGameToPlayer(int clientSocket, int clientId) {
    
    std::shared_ptr<Game> game = gameRooms[clientGameRoomId[clientId]]->getGame(clientId);

    json message;
    
    message["score"] = game->getScore().scoreToJson();
    message["grid"] = game->getGrid().gridToJson();
    message["tetraPiece"] = game->getCurrentPiece().tetraminoToJson(); // Ajout du tétrimino dans le même message

    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0); // Un seul envoi
}

//recuperer les inputs du client
void Server::receiveInputFromClient(int clientSocket, int clientId) {
    std::thread inputThread([this, clientSocket, clientId]() {
        char buffer[1024];

        while (true) {
            if (clientStates[clientId] != MenuState::Play) {
                break;
            }
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); 
            if (bytesReceived > 0) {
                try {
                    json receivedData = json::parse(buffer);
                    std::string action = receivedData["action"];

                    std::cout << "Action reçue du client " << clientId << " : " << action << std::endl;
                    handleMenu(clientSocket, clientId, action);
                } 
                catch (json::parse_error& e) {
                    std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
                }
            }
        }
    });

    inputThread.detach(); // Permet d’exécuter en parallèle
}



void Server::sendChatModeToClient(int clientSocket) {
    json message;
    message["mode"] = "chat";
    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0);
}

bool Server::getRunningChat(int clientSocket) {
    auto it = runningChats.find(clientSocket);
    if (it != runningChats.end()) {
        return it->second;
    }
    return false;
}

void Server::setRunningChat(int clientSocket, bool value) {
    runningChats[clientSocket] = value;
}

void Server::setClientState(int clientId, MenuState state) {
    clientStates[clientId] = state;
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


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


Server::Server(int port) 
    : port(port), serverSocket(-1), clientIdCounter(0), gameRoomIdCounter(0) {
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
    std::mutex clientMutex; // Protect shared resources for this client

    try {
        auto lastRefreshTime = std::chrono::steady_clock::now();

        while (true) {

            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);

            if (bytesReceived == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // Refresh the menu 30 times per second
                    auto currentTime = std::chrono::steady_clock::now();
                    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRefreshTime).count();
                    if (elapsedTime >= 15) { // ~16ms for 15 FPS
                        refreshMenu(clientSocket, clientId);
                        lastRefreshTime = currentTime;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                } else {
                    std::cerr << "Erreur lors de la réception des données du client #" << clientId << ": " << strerror(errno) << std::endl;
                    break;
                }
            }

            if (bytesReceived == 0) {
                std::cout << "Client #" << clientId << " déconnecté." << std::endl;
                return;
            }

            try {
                std::cout << "Client #" << clientId << " a envoyé: " << buffer << std::endl;
                json receivedData = json::parse(buffer);

                if (!receivedData.contains(jsonKeys::ACTION) || !receivedData[jsonKeys::ACTION].is_string()) {
                    std::cerr << "Erreur: 'action' manquant ou invalide dans le JSON reçu." << std::endl;
                    continue;
                }

                std::string action = receivedData[jsonKeys::ACTION];
                handleGUIActions(clientSocket, clientId, receivedData);
                handleMenu(clientSocket, clientId, action);
            } catch (json::parse_error& e) {
                std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception dans handleClient pour le client #" << clientId << ": " << e.what() << std::endl;
    }

}



void Server::handleGUIActions(int clientSocket, int clientId, const json& action) {
    if(action.contains(jsonKeys::ACTION) && action[jsonKeys::ACTION].is_string()) {
        
        std::string actionType = action[jsonKeys::ACTION];
        std::cout << "Client #" << clientId << " a demandé l'action: " << actionType << std::endl;

        if(actionType == "login") {
            //gerer le login
            if(userManager->userNotExists(action["username"])){
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Le pseudo n'existe pas.");
            }else{
                if (userManager->authenticateUser(action["username"], action["password"])) { // Si le mot de passe est correct
                    clientStates[clientId] = MenuState::Main;
                    menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans le menu principal.");
                    clientPseudo[clientId] = action["username"];
                    pseudoTosocket[clientPseudo[clientId]] = clientSocket;
                    sockToPseudo[clientSocket] = clientPseudo[clientId];
                    std::cout << "Client #" << clientId << " connecté avec succès." << std::endl;
                }else{
                    std::cout << "Le mot de passe est incorrect." << std::endl;
                    menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Le mot de passe est incorrect.");
                    
                }
            }
        }
        else if(actionType == "registre") {
            //gerer l'enregistrement
            std::cout << "Client #" << clientId << " a demandé de s'enregistrer." << std::endl;
            if(userManager->userNotExists(action["username"])){
                std::cout << "Le pseudo n'existe pas." << std::endl;
                userManager->registerUser(action["username"], action["password"]);
                clientStates[clientId] = MenuState::Main;
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Le pseudo n'existe pas.");
                clientPseudo[clientId] = action["username"];
                sockToPseudo[clientSocket] = action["username"];
                pseudoTosocket[action["username"]] = clientSocket;
            }else{
                std::cout << "Le pseudo existe déjà." << std::endl;
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Le pseudo existe déjà.");
            }
        }
        else if(actionType == "welcome") {
            //gerer le menu d'accueil
            std::cout << "Client #" << clientId << " a demandé d'ouvrir le menu d'accueil." << std::endl;
            clientStates[clientId] = MenuState::Welcome;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans le menu d'accueil.");
            return;
        }
        else if(actionType == "main"){
            //gerer le menu principal
            std::cout << "Client #" << clientId << " a demandé d'ouvrir le menu principal." << std::endl;
            clientStates[clientId] = MenuState::Main;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans le menu principal.");
            return;
        }
        else if(actionType == "game") {
            //gerer le jeu
            std::cout << "Client #" << clientId << " a demandé d'ouvrir le jeu." << std::endl;
            clientStates[clientId] = MenuState::Game;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans le jeu.");
            return;
        }
        else if(actionType == "classement") {
            //gerer le classement
            std::cout << "Client #" << clientId << " a demandé d'ouvrir le classement." << std::endl;
            clientStates[clientId] = MenuState::classement;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans le classement.");
            return;
        }
        else if(actionType == "settings") {
            //gerer les parametres
            std::cout << "Client #" << clientId << " a demandé d'ouvrir les paramètres." << std::endl;
            clientStates[clientId] = MenuState::Settings;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans les paramètres.");
            return;
        }
        else if(actionType == "teams") {
            //gerer les equipes
            std::cout << "Client #" << clientId << " a demandé d'ouvrir les équipes." << std::endl;
            clientStates[clientId] = MenuState::ManageRooms;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans les équipes.");
            return;
        }
        else if(actionType == "chat") {
            //gerer le chat
            std::cout << "Client #" << clientId << " a demandé d'ouvrir le chat." << std::endl;
            clientStates[clientId] = MenuState::chat;
            auto friends = friendList->getFriendList(clientPseudo[clientId]);
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans le chat.", friends);
            return;
        }
        else if(actionType == "friends") {
            //gerer les amis
            std::cout << "Client #" << clientId << " a demandé d'ouvrir la liste d'amis." << std::endl;
            clientStates[clientId] = MenuState::Friends;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans la liste d'amis.");
            return;
        }
        else if(actionType == "friendsList"){
            //la liste des amis
            auto friends = friendList->getFriendList(clientPseudo[clientId]);
            if(friends.empty()){
                menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Aucun ami trouvé.");
                return;
            }
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Liste d'amis", friends);
            std::cout << "Client #" << clientId << " a demandé d'ouvrir la liste d'amis." << std::endl;
        }
        else if(actionType == "addFriend"){
            //gerer l'ajout d'amis
            keyInputAddFriendMenu(clientSocket, clientId, action["friend"]);
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Demande d'ami envoyée.");
            std::cout << "Client #" << clientId << " a demandé d'ajouter un ami." << std::endl;
            return;
        }
        else if(actionType == "openChat"){
            //gerer l'ouverture du chat
            std::string contact = action["contact"];
            std::cout << "Client #" << clientId << " a demandé d'ouvrir le chat avec " << contact << "." << std::endl;
            //chat->processClientChat(clientSocket, clientId, *this, clientStates[clientId]);
            return;
        }
        else if(actionType == "createjoin"){
            std::cout << "Client #" << clientId << " a demandé d'ouvrir le play." << std::endl;
            clientStates[clientId] = MenuState::JoinOrCreateGame;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans menu play.");
            return;
        }
        else if(actionType == "choiceMode"){
            std::cout << "Client #" << clientId << " a demandé de choisir le mode de jeu ." << std::endl;
            clientStates[clientId] = MenuState::CreateGame;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans menu modeJeu .");
            return;
        }
        else if(actionType == "EndlessMode"){
            clientStates[clientId] = MenuState::Play;
            this->keyInputCreateGameRoom(clientId, GameModeName::Endless);
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans menu modeJeu Endless.");
         
            
            
        }
        else if(actionType == "DuelMode") {
            clientStates[clientId] = MenuState::Settings;
            
        }
        else if(actionType == "rejouer"){
            clientStates[clientId] = MenuState::JoinOrCreateGame;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans menu play.");
            return;
        }

        else if(actionType == "retour au menu"){
            clientStates[clientId] = MenuState::Main;
            menuStateManager->sendMenuStateToClient(clientSocket, clientStates[clientId], "Bienvenue dans le menu principal.");
            return;

        }
        
        
    }
}

void Server::disconnectPlayer(int clientId){
    close(clientIdToSocket[clientId]); // Fermer la connexion
    clientGameRoomId.erase(clientId); // Supprimer l'ID de la salle de jeu   
    clientIdToSocket.erase(clientId); // Supprimer le socket du client
    clientStates.erase(clientId); // Supprimer l'état des menus du client
}

void Server::handleMenu(int clientSocket, int clientId, const std::string& action) {
    switch (clientStates[clientId]) {
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
            keyInputManageRoom(clientSocket, clientId, action);
            break;
        case MenuState::ListRoomMembres:
            keyInputListMembres(clientSocket, clientId, action);
            break;
        case MenuState::AddMembre:
            keyInputAddMembre(clientSocket, clientId, action);
            break;
        case MenuState::AddAdmin:
            keyInputAddAdmin(clientSocket, clientId, action);
            break;
        case MenuState::RoomRequestList:
            keyInputRequestList(clientSocket, clientId, action);
            break;
        case MenuState::ConfirmDeleteRoom:
            keyInputConfirmDeleteRoom(clientSocket, clientId, action);
            break;
        case MenuState::ConfirmQuitRoom:
            keyInputQuitRoom(clientSocket, clientId, action);
            break;
        case MenuState::Friends:
            keyInputFriendsMenu(clientSocket, clientId, action);
            break;
        case MenuState::AddFriend:
            keyInputAddFriendMenu(clientSocket, clientId, action);
            break;
        case MenuState::FriendList:
            keyInputManageFriendlist(clientSocket, clientId, action);
            break;
        case MenuState::FriendRequestList:
            keyInputManageFriendRequests(clientSocket, clientId, action);
            break;
        case MenuState::JoinOrCreateGame:
            keyInputJoinOrCreateGameMenu(clientSocket, clientId, action);
            break;
        case MenuState::JoinGame:
            keyInputChoiceGameRoom(clientSocket, clientId, action);
            break;
        case MenuState::CreateGame:
            keyInputChooseGameModeMenu(clientSocket, clientId, action);
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

void Server::refreshMenu(int clientSocket, int clientId) {
    std::string currentUser = sockToPseudo[clientSocket];
    std::string currentRoom = roomToManage[clientId];
    MenuState state = clientStates[clientId];

    switch (state) {
        case MenuState::Welcome:
            sendMenuToClient(clientSocket, menu.getMainMenu0());
            break;
        case MenuState::Main:
            sendMenuToClient(clientSocket, menu.getMainMenu1());
            break;
        case MenuState::ManageRooms:
            sendMenuToClient(clientSocket, menu.getManageChatRoomsMenu(chat->getMyRooms(clientPseudo[clientId])));
            break;
        case MenuState::ManageRoom:
            sendMenuToClient(clientSocket, menu.getManageRoomMenu(chatRooms[currentRoom]->isAdmin(currentUser), chatRooms[currentRoom]->getadminPseudo().size() == 1));
            break;
        case MenuState::ListRoomMembres:
            sendMenuToClient(clientSocket, menu.getListeMembers(chatRooms[currentRoom]->getClients()));
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
            case MenuState::chat:
            sendMenuToClient(clientSocket, menu.getChatMenu());
            break;
        case MenuState::CreateRoom:
            sendMenuToClient(clientSocket, menu.getCreateChatRoomMenu());
            break;
        case MenuState::JoinRoom:
            sendMenuToClient(clientSocket, menu.getJoinChatRoomMenu());
            break;
        case MenuState::AddMembre:
            sendMenuToClient(clientSocket, menu.getAddMembreMenu());
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


void Server::returnToMenu(int clientSocket, int clientId, MenuState state, const std::string& message, int sleepTime ) {
    if (!message.empty()) {
        sendMenuToClient(clientSocket, menu.displayMessage(message));
        sleep(sleepTime);
    }
    clientStates[clientId] = state;
    std::string currentUser = clientPseudo[clientId];
    std::string currentRoom = roomToManage[clientId];

    refreshMenu(clientSocket, clientId);
}



void Server::keyInputManageFriendRequests(int clientSocket, int clientId, const std::string& action) {
    std::string currentUser = clientPseudo[clientId];

    if (action == "/quit") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
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
    std::cout << "Client #" << sockToPseudo[clientId] << " a demandé de gérer la liste d'amis." << std::endl;
    if (action == "/quit") {
        returnToMenu(clientSocket, clientId, MenuState::Friends);
        return;
    }

    //test//
    /*if (friendList->getFriendList(currentUser).empty()) {
        friendList->createDummyFriends(currentUser);
        return;
    }*/

    std::vector<std::string> friends = friendList->getFriendList(currentUser);
    for (const std::string& friend_name : friends) {
        std::cout << "Ami: " << friend_name << std::endl;
    }
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
        std::cout << "Client #" << sockToPseudo[clientId] << " a demandé de gérer la liste d'amis." << std::endl;
        
    }
    else if (action == "3") {
        returnToMenu(clientSocket, clientId, MenuState::FriendRequestList);
    
    }
    else if (action == "4") {
        returnToMenu(clientSocket, clientId, MenuState::Main);
    }

}

void Server::keyInputAddFriendMenu(int clientSocket, int clientId, const std::string& action) {
    if (action == "/quit") {
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
                    userManager->updateHighscore(clientPseudo[player], gameRoom->getScoreValue());
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
        
        clientStates[clientId] = MenuState::JoinOrCreateGame;
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
        
    }
    else if(action == "2") {
        clientStates[clientId] = MenuState::JoinRoom;
        
    }
    else if(action == "3") {
        clientStates[clientId] = MenuState::ManageRooms;
    }
    else if(action == "4") {
        sendChatModeToClient(clientSocket);
        //chat->processClientChat(clientSocket, clientId, *this, MenuState::chat, menu.getChatMenu());
    }
    else if(action == "5") {
        clientStates[clientId] = MenuState::Main;
    }
}


void Server::keyInputCreateChatRoom(int clientSocket, int clientId, const std::string& action) {
    if(action == "/quit") {
        returnToMenu(clientSocket, clientId, MenuState::chat);
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
    if(action == "/quit") {
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
    if(action == "/quit") {
        clientStates[clientId] = MenuState::chat;
    }
    else if(std::find(myRooms.begin(), myRooms.end(), action) == myRooms.end()) {
        returnToMenu(clientSocket, clientId, MenuState::chat, "Room name does not exist.");
    } 
    else {
        roomToManage[clientId] = action;
        clientStates[clientId] = MenuState::ManageRoom;
    }
}

void Server::keyInputManageRoom(int clientSocket, int clientId, const std::string& action) {
    std::string roomName = roomToManage[clientId];
    if(action == "/quit") {
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
    if(action == "/quit") {
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
    if(action == "/quit") {
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
    if(action == "/quit") {
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
    if(action == "/quit") {
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

    if(action == "/quit") {
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
    std::string msg = message.dump() + "\n";
    send(clientSocket, msg.c_str(), msg.size(), 0);
}

void Server::setClientState(int clientId, MenuState state) {
    clientStates[clientId] = state;
}


////////////////Ce qui suit est pour les invitations de jeu/////////////////
//************************************************************************ */


void Server::keyInputSendGameRequestMenu(int clientSocket, int clientId, std::string receiver, std::string status) {
    //### GÉRER L'INVITATION D'UN VIEWER
    std::cout << "Sending game request to: " << receiver << " with status: " << status << std::endl;

    std::string game_request = receiver;
    std::string currentUser = clientPseudo[clientId];

    if(game_request == currentUser){
        sendMenuToClient(clientSocket, menu.displayMessage("Erreur : Vous ne pouvez pas vous envoyer une invitation de jeu."));
        sleep(3);
        return;
    }
    if(!friendList->areFriends(currentUser,game_request)){
        sendMenuToClient(clientSocket, menu.displayMessage("Erreur : Vous n'êtes pas ami avec "+game_request+"."));
        sleep(3);
        return;
    }
    int gameRoomId = clientGameRoomId[clientId];
    friendList->sendInvitationToFriend(currentUser, game_request,status, gameRoomId);
    sendMenuToClient(clientSocket, menu.displayMessage("Request Sent To "+game_request+"."));
    sleep(3);

}


void Server::keyInputHelpMenu(int clientSocket, int clientId, const std::string& action) {
    auto gameRoom = gameRooms[clientGameRoomId[clientId]];
    if (action.find("/quit") == 0){
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

    std::cout << "Received action: " << action << " from client #" << clientId << std::endl;
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
    
    if (action.find("/quit") == 0){
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
        clientGameRoomId[clientId] = roomNumber;
        auto gameRoom = gameRooms[roomNumber];
        gameRoom->addPlayer(clientId);
        clientStates[clientId] = MenuState::Settings;
        
        for (const auto& cId : gameRooms[roomNumber]->getPlayers())
            sendMenuToClient(pseudoTosocket[clientPseudo[cId]], menu.getLobbyMenu(getMaxPlayers(cId), getMode(cId), getAmountOfPlayers(cId), gameRoom->getSpeed(), gameRoom->getEnergyLimit()));

    }
    else if(action.find("/quit") == 0){
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

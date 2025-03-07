#include "ClientChat.hpp"
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>

using json = nlohmann::json;
std::mutex mtx; // Mutex pour synchroniser l'accès aux fenêtres ncurses

#define INPUT_HEIGHT 3  // Hauteur de la zone de saisie
bool ClientChat::messagesWaitForDisplay = false;

void ClientChat::run() {
    initscr();       // Initialise ncurses
    cbreak();        // Désactive le buffering de ligne
    noecho();        // Empêche l'affichage automatique des entrées utilisateur
    keypad(stdscr, TRUE); // Active la gestion des touches spéciales
    scrollok(stdscr, TRUE); // Permet le défilement du texte

    int height, width;
    getmaxyx(stdscr, height, width); // Récupère les dimensions du terminal

    // Fenêtre pour afficher les messages (en haut)
    displayWin = newwin(height - 3, width, 0, 0); // Hauteur : height - 3, Largeur : width
    scrollok(displayWin, TRUE); // Activer le défilement pour cette fenêtre
    box(displayWin, 0, 0); // Dessiner une bordure autour de la fenêtre
    wrefresh(displayWin); // Rafraîchir la fenêtre

    // Fenêtre pour saisir les messages (en bas)
    inputWin = newwin(INPUT_HEIGHT, width, height - INPUT_HEIGHT, 0);
    scrollok(inputWin, TRUE);
    box(inputWin, 0, 0);
    wrefresh(inputWin);

    if (!initMessageMemory()) {
        std::cerr << "Erreur lors de l'initialisation de la mémoire des messages !\n";
        endwin();
        return;
    }

    std::thread sendThread(&ClientChat::sendChatMessages, this);
    sendThread.join();  // Attendre la fin du thread avant de détruire l'objet

    delwin(displayWin);
    delwin(inputWin);
    echo(); // Réactiver l'affichage automatique des entrées utilisateur
    endwin();
}

void ClientChat::sendChatMessages() {
    isChatting = true;
    std::string inputStr;
    std::string constReceiver;
    int ch;
    while (true) {
        mtx.lock();
        werase(inputWin);
        box(inputWin, 0, 0);
        mvwprintw(inputWin, 1, 1, "> %s", inputStr.c_str()); // Afficher le buffer de saisie
        wrefresh(inputWin);
        mtx.unlock();

        std::string receiver, message;

        ch = getch();
        if (ch == 10) {  // Entrée
            if (inputStr.empty()) continue;

            if(constReceiver.empty() && inputStr.substr(0, 2) != "./"){
                std::cerr << "Veuillez spécifier un destinataire !\n";
                continue;
            }

            //si le message est ./flush on affiche tous les messages enregistrés non affichés
            if(inputStr == "./flush"){
                if (FlushMemory()) {
                    y = 1;
                    continue;
                }
            }

            //si le message est ./exit on envoie un message au serveur pour lui dire qu'on veut quitter le chat
            if(inputStr == "./exit"){
                receiver = "server";
                message = "exit";
            }
            //sinon on envoie un message normal (après avoir vérifié le format)
            else{
                size_t pos = inputStr.find(' ');
                if (pos == std::string::npos) {
                    if(inputStr.substr(0, 2) == "./"){
                        constReceiver = inputStr.substr(2, pos - 2);
                        inputStr.clear();
                        continue;
                    }else{
                        message = inputStr;
                    }
                }else{
                    receiver = inputStr.substr(2, pos - 2);
                    message = inputStr.substr(pos + 1);
                }
                
            }

            if(message.empty()){
                std::cerr << "Veuillez spécifier un message !\n";
                continue;
            }

            if(receiver.empty() && !constReceiver.empty()){
                receiver = constReceiver;
            }
            json msg_json = { {"receiver", receiver}, {"message", message} };

            if (!network.sendData(msg_json.dump(), clientSocket)) {
                std::cerr << "Erreur d'envoi du message !\n";
            }
            if (receiver == "server" && message == "exit") {
                break;
            }
            displayChatMessage("Moi->"+receiver, message);
            y++;
            inputStr.clear();

        } else if (ch == 127 || ch == KEY_BACKSPACE) {
            if (!inputStr.empty()) inputStr.pop_back();
        } else if (isprint(ch)) {
            inputStr += static_cast<char>(ch);
        }
    }
    isChatting = false;
    std::cout << "Fin du thread d'envoi de messages !" << std::endl;
}

void ClientChat::receiveChatMessages(const json& msg) {

    //if (!isChatting)
            saveMessage(msg.dump());
    //else{
            displayChatMessage(msg["sender"], msg["message"]);y++; //sinon on l'affiche directement
    //}
                
}

void ClientChat::displayChatMessage(std::string sender, const std::string& message) {
    mtx.lock();// Verrouiller l'accès à la fenêtre ncurses
    mvprintw(y, 1, "[%s] : %s", sender.c_str(), message.c_str());
    refresh();
    mtx.unlock();// Déverrouiller l'accès
}

bool ClientChat::initMessageMemory() {
    std::ifstream file("messages.json");
    if (!file.good()) {
        std::ofstream newFile("messages.json");
        if (newFile.is_open()) {
            newFile.close();
            return true;
        }
        std::cerr << "Erreur lors de la création du fichier messages.json." << std::endl;
        return false;
    }
    return true;
}

bool ClientChat::saveMessage(const std::string& message) {
    std::ofstream file("messages.json", std::ios::app);
    if (file.is_open()) {
        file << message << std::endl;
        file.close();
        messagesWaitForDisplay = true;
        return true;
    }
    return false;
}

bool ClientChat::FlushMemory() {
    std::ifstream file("messages.json");
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            json message = json::parse(line);
            std::cout << message.dump(4) << std::endl;
        }
        file.close();
        std::ofstream clearFile("messages.json", std::ios::trunc);
        messagesWaitForDisplay = false;
        return true;
    }
    return false;
}

void ClientChat::setIsChatting(bool isChatting) {
    this->isChatting = isChatting;
}

void ClientChat::setClientSocket(int clientSocket) {
    this->clientSocket = clientSocket;
}

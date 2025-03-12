#include "ClientChat.hpp"
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>


std::mutex mtx; // Mutex pour synchroniser l'accès aux fenêtres ncurses
#define INPUT_HEIGHT 3  // Hauteur de la zone de saisie


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

    std::thread sendThread(&ClientChat::sendChatMessages, this);
    sendThread.join();  // Attendre la fin du thread d'envoi de messages

    delwin(displayWin);
    delwin(inputWin);
    echo(); // Réactiver l'affichage automatique des entrées utilisateur
    endwin(); // Terminer ncurses
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
                receiver = "server";
                message = "flush";
            }

            //si le message est ./exit on envoie un message au serveur pour lui dire qu'on veut quitter le chat
            if(inputStr == "./exit"){
                receiver = "server";
                message = "exit";
            }
            //sinon on envoie un message normal (après avoir vérifié le format)
            else{
                if(constReceiver.empty()){
                    size_t pos = inputStr.find(' ');
                    if (pos == std::string::npos) {
                        if(inputStr.substr(0, 2) == "./"){
                            constReceiver = inputStr.substr(2, pos - 2);
                            inputStr.clear();
                            continue;
                        }else{
                            std::cerr << "Veuillez spécifier un destinataire !\n";
                            continue;
                        }
                    }else{
                        receiver = inputStr.substr(2, pos - 2);
                        message = inputStr.substr(pos + 1);
                    }
                }else{
                    if (inputStr.substr(0, 2) == "./") {
                        int pos = inputStr.find(' ');
                        receiver = inputStr.substr(2, pos - 2);
                        message = inputStr.substr(pos + 1);
                    }else{
                        receiver = constReceiver;
                        message = inputStr;
                    }
                }
            }
            if(message.empty()){
                std::cerr << "Veuillez spécifier un message !\n";
                continue;
            }

            json msg_json = { {"receiver", receiver}, {"message", message} };
            std::cout << msg_json << std::endl;
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
    y = 1;
    isChatting = false;
}

void ClientChat::receiveChatMessages(const json& msg) {
    displayChatMessage(msg["sender"], msg["message"]);
    y++;        
}

void ClientChat::displayChatMessage(std::string sender, const std::string& message) {
    mtx.lock();// Verrouiller l'accès à la fenêtre ncurses
    mvprintw(y, 1, "[%s] : %s", sender.c_str(), message.c_str());
    refresh();
    mtx.unlock();// Déverrouiller l'accès
}

void ClientChat::setIsChatting(bool isChatting) {
    this->isChatting = isChatting;
}

void ClientChat::setClientSocket(int clientSocket) {
    this->clientSocket = clientSocket;
}

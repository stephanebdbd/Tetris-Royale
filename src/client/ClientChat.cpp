#include "ClientChat.hpp"
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>


std::mutex mtx; // Mutex pour synchroniser l'accès aux fenêtres ncurses
#define INPUT_HEIGHT 6  // Hauteur de la zone de saisie


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
    scrollok(inputWin, TRUE); // Activer le défilement pour cette fenêtre
    box(inputWin, 0, 0); // Dessiner une bordure autour de la fenêtre
    curs_set(1); // Rendre le curseur visible
    wrefresh(inputWin); // Rafraîchir la fenêtre

    std::thread sendThread(&ClientChat::sendChatMessages, this);
    sendThread.join();  // Attendre la fin du thread d'envoi de messages

    delwin(displayWin);
    delwin(inputWin);
    echo(); // Réactiver l'affichage automatique des entrées utilisateur
    curs_set(0); // Rendre le curseur invisible
    endwin(); // Terminer ncurses
}


void ClientChat::sendChatMessages() {
    std::string inputStr;
    std::string constReceiver;
    json msg_json;
    int ch;

    while (true) {
        mtx.lock();
        werase(inputWin);
        box(inputWin, 0, 0);
        mvwprintw(inputWin, 1, 1, "> %s", inputStr.c_str()); // Afficher le buffer de saisie
        wrefresh(inputWin);
        mtx.unlock();

        ch = getch();
        if (ch == 10) {  // Entrée
            if (inputStr.empty()) continue;

            msg_json = {
                {"message", inputStr}
            };
            
            if (!network.sendData(msg_json.dump(), clientSocket)) {
                std::cerr << "Erreur d'envoi du message !\n";
            }
            if (inputStr == "/exit") {
                break;
            }
            displayChatMessage("You", inputStr);
            inputStr.clear();

        } else if (ch == 127 || ch == KEY_BACKSPACE) {
            if (!inputStr.empty()) inputStr.pop_back();
        } else if (isprint(ch)) {
            inputStr += static_cast<char>(ch);
        }
    }
    y = 1;
}

/*
a supprimer 
*/
void ClientChat::receiveChatMessages(const json& msg) {
    displayChatMessage(msg["sender"], msg["message"]);      
}

void ClientChat::displayChatMessage(std::string sender, const std::string& message) {
    mtx.lock();// Verrouiller l'accès à la fenêtre ncurses
    if(y == LINES - INPUT_HEIGHT - 1){
        wclear(displayWin);
        box(displayWin, 0, 0);
        y = 1;
        wrefresh(displayWin);
    }
    mvwprintw(displayWin, y++, 1, "[%s] : %s", sender.c_str(), message.c_str());
    wrefresh(displayWin);
    mtx.unlock();// Déverrouiller l'accès
}

void ClientChat::setClientSocket(int clientSocket) {
    this->clientSocket = clientSocket;
}

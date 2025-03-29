#include "SFMLGame.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

SFMLGame::SFMLGame(Client& client) : client(client) {}

void SFMLGame::run() {
    if (!client.connect()) {
        std::cerr << "Erreur: Impossible de se connecter au serveur." << std::endl;
        return;
    }
    sf::RenderWindow window(sf::VideoMode(800, 600), "Tetris Royal - Graphique");

    std::thread inputThread(&Client::handleUserInput, &client);
    inputThread.detach();
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                client.sendSFMLInput(event.key.code);
            }
        }

        window.clear(sf::Color::Black);
        // TODO: dessiner la grille, les pièces, etc.
        window.display();
    }
}

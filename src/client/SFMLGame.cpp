#include "SFMLGame.hpp"
#include <SFML/Graphics.hpp>

SFMLGame::SFMLGame(Client& client) : client(client) {}

void SFMLGame::run() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Tetris Royal - Graphique");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            // pour gérer le clavier
        }

        window.clear(sf::Color::Black);
        // TODO: dessiner la grille, les pièces, etc.
        window.display();
    }
}

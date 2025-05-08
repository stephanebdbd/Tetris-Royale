#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <memory>
#include <string>

class Rectangle {
    sf::RectangleShape shape;
public:
    Rectangle(const sf::Vector2f& position, const sf::Vector2f& size, 
              const sf::Color& fillColor, const sf::Color& outlineColor);

    void draw(sf::RenderWindow& window) const;
    void setTexture(const sf::Texture& texture);
    sf::Vector2f getPosition() const;
};

class Circle {
    sf::CircleShape shape;
public:
    Circle(const sf::Vector2f& position, float radius, 
           const sf::Color& fillColor, const sf::Color& outlineColor);

    void draw(sf::RenderWindow& window) const;
    void setTexture(const sf::Texture& texture);
};

class Button {
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color originalColor;
    sf::Color hoverColor;
    sf::Color clickColor;
    bool wasPressed = false;
    sf::Clock clickTimer;
    std::shared_ptr<sf::Texture> buttonTexture;
    unsigned int baseCharSize = 30;
public:
    // Pour le Texte
    Button(const std::string& text, const sf::Font& font, unsigned int characterSize,
           const sf::Color& textColor, const sf::Color& backgroundColor,
           const sf::Vector2f& position, const sf::Vector2f& size,
           const sf::Color& outlineColor = sf::Color::Black);

    // Pour l'Image
    Button(const sf::Texture& texture, const sf::Vector2f& position, const sf::Vector2f& size);

    void draw(sf::RenderWindow& window) const;
    bool isMouseOver(const sf::RenderWindow& window) const;
    bool isClicked(sf::RenderWindow& window);
    void update();
    void setBackgroundColor(sf::RenderWindow& window);
    void resetColor();
    std::string getText() const;
    void resize(float scaleX, float scaleY);
    void setTexture(const sf::Texture& texture);
    bool operator==(const Button& other) const;
}; 

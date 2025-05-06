#pragma once

#include <SFML/Graphics.hpp>

class Text{
    public:
        Text(const std::string& text, const sf::Font& font, unsigned int characterSize, 
             const sf::Color& textColor, const sf::Vector2f& position, sf::Uint32 style = sf::Text::Bold);
    
        void draw(sf::RenderWindow& window) const;
        sf::FloatRect getLocalBounds() const;
    
    private:
        sf::Text text;
};

// Classe pour les champs de texte
class TextField {
    public:
        TextField(const sf::Font& font, unsigned int characterSize, 
                  const sf::Color& textColor, const sf::Color& backgroundColor, 
                  const sf::Vector2f& position, const sf::Vector2f& size, 
                  const std::string& placeholder, bool hidden = false);
    

        void draw(sf::RenderWindow& window) const;
        bool isMouseOver(const sf::RenderWindow& window) const;
        void setActive(bool active);
        void handleInput(sf::Event event);
        std::string getText() const;
        void updateCursor();
        void validateInput();
        void clear();
        void resize(float scaleX, float scaleY) ;
        void setText(const std::string& newText);

    
    private:
        sf::RectangleShape shape;
        sf::Text text;
        sf::Text placeholderText;
        std::string textString;
        std::string placeholder;
        bool isActive;
        unsigned int maxLength;
        bool showCursor;
        sf::Clock cursorClock;
        sf::RectangleShape cursor;
        float padding;
        bool hiddenText = false;
        
};
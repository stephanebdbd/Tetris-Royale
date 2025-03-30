#include "Button.hpp"


Button::Button(const std::string& text, const sf::Font& font, unsigned int characterSize, 
        const sf::Color& textColor, const sf::Color& backgroundColor, 
        const sf::Vector2f& position, const sf::Vector2f& size)
    {

        // Configuration de la couleur d'origine
        this->originalColor = backgroundColor;
        this->hoverColor = sf::Color(backgroundColor.r-30, backgroundColor.g-30, backgroundColor.b-30, 150);
        this->clickColor = sf::Color(backgroundColor.r-50, backgroundColor.g-50, backgroundColor.b-50, 150);
        
        // Configuration du rectangle
        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(backgroundColor);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::Black);

        // Configuration du texte
        this->text.setFont(font);
        this->text.setString(text);
        this->text.setCharacterSize(characterSize);
        this->text.setFillColor(textColor);

        // Centrer le texte dans le bouton
        sf::FloatRect textRect = this->text.getLocalBounds();
        this->text.setOrigin(textRect.left + textRect.width / 2.0f, 
                            textRect.top + textRect.height / 2.0f);
        this->text.setPosition(position.x + size.x / 2.0f, 
                            position.y + size.y / 2.0f);
    }

void Button::draw(sf::RenderWindow& window) const {
    window.draw(shape);
    window.draw(text);
}

bool Button::isMouseOver(const sf::RenderWindow& window) const {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    return shape.getGlobalBounds().contains(mousePos);
}

bool Button::isMousePressed(const sf::RenderWindow& window) const {
    return isMouseOver(window) && sf::Mouse::isButtonPressed(sf::Mouse::Left);
}

void Button::setBackgroundColor(sf::RenderWindow& window) {
    if(isMouseOver(window)) {
        if (isMousePressed(window)) {
            shape.setFillColor(clickColor);
        } else {
            shape.setFillColor(hoverColor);
        }
    } else {
        shape.setFillColor(originalColor);
    }
}

void Button::resetColor() {
    shape.setFillColor(originalColor);
}
#include "Text.hpp"


TextField::TextField(const sf::Font& font, unsigned int characterSize, 
    const sf::Color& textColor, const sf::Color& backgroundColor, 
    const sf::Vector2f& position, const sf::Vector2f& size, 
    const std::string& placeholder)
    : shape(), text(), placeholderText(), textString(""), placeholder(placeholder), isActive(false), maxLength(50), showCursor(false), padding(10.0f)
{
    // Configuration du rectangle
    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(backgroundColor);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::Black);

    // Configuration du curseur
    cursor.setSize(sf::Vector2f(2, characterSize));
    cursor.setFillColor(textColor);
    

    // Configuration du texte
    text.setFont(font);
    text.setCharacterSize(characterSize);
    text.setFillColor(textColor);
    text.setPosition(position.x + padding, position.y + (size.y - characterSize) / 2);

    // Configuration du texte placeholder
    placeholderText.setFont(font);
    placeholderText.setCharacterSize(characterSize);
    placeholderText.setFillColor(sf::Color(150, 150, 150));
    placeholderText.setString(placeholder);
    placeholderText.setPosition(position.x + padding, position.y + (size.y - characterSize) / 2);
}



bool TextField::isMouseOver(const sf::RenderWindow& window) const {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    return shape.getGlobalBounds().contains(mousePos);
}

void TextField::draw(sf::RenderWindow& window) const {
    window.draw(shape);
    
    if (textString.empty() && !isActive) {
        window.draw(placeholderText);
    } else {
        window.draw(text);
        if (isActive && showCursor) {
            window.draw(cursor);
        }
    }
}

void TextField::setActive(bool active) {
    isActive = active;
    cursorClock.restart();
    showCursor = active;
    
    if (active) {
        shape.setOutlineColor(sf::Color::Blue);
        // Position cursor at end of text
        cursor.setPosition(text.findCharacterPos(textString.size()).x, text.getPosition().y);
    } else {
        shape.setOutlineColor(sf::Color::Black);
    }
}

void TextField::updateCursor() {
    if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
        showCursor = !showCursor;
        cursorClock.restart();
    }
}

void TextField::handleInput(sf::Event event) {
    if (!isActive) return;

    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode < 128) {
            if (event.text.unicode == '\b') {
                if (!textString.empty()) {
                    textString.pop_back();
                }
            } else if (textString.size() < maxLength && 
                      event.text.unicode != '\r' && 
                      event.text.unicode != '\t') {
                textString += static_cast<char>(event.text.unicode);
            }
            text.setString(textString);
            cursor.setPosition(text.findCharacterPos(textString.size()).x, text.getPosition().y);
        }
    }
}

std::string TextField::getText() const {
    return textString;
}

void TextField::clear() {
    textString.clear();
    text.setString(textString);
}
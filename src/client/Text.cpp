#include "Text.hpp"


TextField::TextField(const sf::Font& font, unsigned int characterSize, 
    const sf::Color& textColor, const sf::Color& backgroundColor, 
    const sf::Vector2f& position, const sf::Vector2f& size, 
    const std::string& placeholder, bool hidden)
    : shape(), text(), placeholderText(), textString(""), placeholder(placeholder), isActive(false), maxLength(250), showCursor(false), padding(10.0f), hiddenText(hidden)
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

        if(hiddenText) {
            std::string maskedText = std::string(textString.size(), '*');
            sf::Text maskedPlaceholder = placeholderText;
            maskedPlaceholder.setString(maskedText.substr(maskedText.size() > 15 ? maskedText.size() - 15 : 0));
            window.draw(maskedPlaceholder);
        } else {
            sf::Text visibleText = text;
            visibleText.setString(textString.substr(textString.size() > 15 ? textString.size() - 15 : 0));
            window.draw(visibleText);   
        }

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
    if (isActive && cursorClock.getElapsedTime().asSeconds() > 0.5f) {
        showCursor = !showCursor;
        cursorClock.restart();
    }

    // Ensure the cursor does not go beyond the text field's boundaries
    float cursorX = cursor.getPosition().x;
    float maxCursorX = shape.getPosition().x + shape.getSize().x - padding;
    if (cursorX > maxCursorX) {
        cursor.setPosition(maxCursorX, cursor.getPosition().y);
    }
}


void TextField::validateInput() {
    // Exemple de validation simple - peut être adapté selon les besoins
    if (textString.empty()) {
        text.setString(placeholder);
        text.setFillColor(sf::Color(150, 150, 150));
    } else {
        text.setString(textString);
        text.setFillColor(sf::Color::Black);
    }
}

void TextField::handleInput(sf::Event event) {
    if (!isActive) return;

    if (event.type == sf::Event::TextEntered) {
        char enteredChar = static_cast<char>(event.text.unicode);

        if (event.text.unicode < 128) {
            if (enteredChar == '\b') { // Backspace
                if (!textString.empty()) {
                    textString.pop_back();
                }
            } else if (textString.size() < maxLength && 
                      enteredChar != '\r' && 
                      enteredChar != '\t') {
                textString += enteredChar;
            }

            // Update text and cursor position only when necessary
            text.setString(hiddenText ? std::string(textString.size(), '*') : textString);
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
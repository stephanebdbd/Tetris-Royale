#include "Text.hpp"

Text::Text(const std::string& text, const sf::Font& font, unsigned int characterSize, 
    const sf::Color& textColor, const sf::Vector2f& position, sf::Uint32 style)
    : text(text, font, characterSize)
{
    this->text.setFillColor(textColor);
    this->text.setPosition(position);
    this->text.setStyle(style);
    this->text.setOutlineThickness(0);
}

void Text::draw(sf::RenderWindow& window) const {
    window.draw(text);
}


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
    // Draw the background shape
    window.draw(shape);

    // Determine whether to draw the placeholder or the actual text
    if (textString.empty() && !isActive) {
        window.draw(placeholderText);
    } else {
        // Calculate the maximum allowed width for the text
        float maxWidth = shape.getSize().x - 2 * padding;

        // Create a copy of the text to display
        sf::Text displayText = text;
        std::string displayString = hiddenText ? std::string(textString.size(), '*') : textString;
        displayText.setString(displayString);

        // Trim the text to fit within the maximum width
        while (displayText.getLocalBounds().width > maxWidth && !displayString.empty()) {
            displayString.erase(0, 1); // Remove the first character
            displayText.setString(displayString);
        }

        // Center the text vertically within the text field
        displayText.setPosition(
            shape.getPosition().x + padding,
            shape.getPosition().y + (shape.getSize().y - displayText.getLocalBounds().height) / 2 - displayText.getLocalBounds().top
        );

        // Draw the text
        window.draw(displayText);

        // Draw the cursor if the text field is active and the cursor is visible
        if (isActive && showCursor) {
            // Calculer la position du curseur
            float cursorX = displayText.findCharacterPos(displayText.getString().getSize()).x;
            float maxCursorX = shape.getPosition().x + shape.getSize().x - padding;
            
            // Créer une vue mutable temporaire du curseur
            const_cast<TextField*>(this)->cursor.setPosition(
                std::min(cursorX, maxCursorX),
                shape.getPosition().y + 5
            );
            
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
void TextField::resize(float scaleX, float scaleY) {
    // Redimensionner la forme du champ de texte
    sf::Vector2f newSize = shape.getSize();
    newSize.x *= scaleX;
    newSize.y *= scaleY;
    shape.setSize(newSize);

    // Repositionner le champ de texte
    sf::Vector2f newPosition = shape.getPosition();
    newPosition.x *= scaleX;
    newPosition.y *= scaleY;
    shape.setPosition(newPosition);

    // Repositionner le texte
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        shape.getPosition().x + 5, // Décalage pour l'alignement
        shape.getPosition().y + (shape.getSize().y - textBounds.height) / 2 - textBounds.top
    );
}
std::string TextField::getText() const {
    return textString;
}

void TextField::clear() {
    textString.clear();
    text.setString(textString);
}
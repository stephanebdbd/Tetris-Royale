// Button.cpp
#include "Button.hpp"

Rectangle::Rectangle(const sf::Vector2f& position, const sf::Vector2f& size, 
                     const sf::Color& FillColor, const sf::Color& outLineColor) {
    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(FillColor);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(outLineColor);
}

void Rectangle::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

Circle::Circle(const sf::Vector2f& position, const sf::Vector2f& size, 
               const sf::Color& FillColor, const sf::Color& outLineColor) {
    shape.setPosition(position);
    shape.setRadius(size.x / 2);
    shape.setFillColor(FillColor);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(outLineColor);
}

void Circle::draw(sf::RenderWindow& window) {
    if(texture) {
        shape.setTexture(texture.get());
    }
    window.draw(shape);
}

void Circle::setTexture(const std::shared_ptr<sf::Texture>& texture) {
    this->texture = texture;
    shape.setTexture(texture.get(), true);
}

Button::Button(const std::string& text, const sf::Font& font, unsigned int characterSize, 
               const sf::Color& textColor, const sf::Color& backgroundColor, 
               const sf::Vector2f& position, const sf::Vector2f& size, 
               const sf::Color& rectangleColor)
    : clicked(false), clickProcessed(false) {
    // Configuration des couleurs
    this->originalColor = backgroundColor;
    this->hoverColor = sf::Color(
        std::max(0, backgroundColor.r-30), 
        std::max(0, backgroundColor.g-30), 
        std::max(0, backgroundColor.b-30), 
        150);
    this->clickColor = sf::Color(
        std::max(0, backgroundColor.r-50), 
        std::max(0, backgroundColor.g-50), 
        std::max(0, backgroundColor.b-50), 
        150);
    
    // Configuration du rectangle
    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(backgroundColor);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(rectangleColor);

    // Configuration du texte
    this->text.setFont(font);
    this->text.setString(text);
    this->text.setCharacterSize(characterSize);
    this->text.setFillColor(textColor);

    // Centrer le texte
    sf::FloatRect textRect = this->text.getLocalBounds();
    this->text.setOrigin(textRect.left + textRect.width / 2.0f, 
                        textRect.top + textRect.height / 2.0f);
    this->text.setPosition(position.x + size.x / 2.0f,
                        position.y + size.y / 2.0f);
}

void Button::draw(sf::RenderWindow& window) const {
    if(buttonTexture) {
        sf::Sprite sprite(*buttonTexture);
        sprite.setPosition(shape.getPosition());
        // Conserver les proportions de l'image
        float scaleX = shape.getSize().x / buttonTexture->getSize().x;
        float scaleY = shape.getSize().y / buttonTexture->getSize().y;
        sprite.setScale(scaleX, scaleY);
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
    window.draw(text);
}

bool Button::isMouseOver(const sf::RenderWindow& window) const {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    return shape.getGlobalBounds().contains(mousePos);
}

bool Button::isClicked(sf::RenderWindow& window) {
    bool isPressed = isMouseOver(window) && sf::Mouse::isButtonPressed(sf::Mouse::Left);
    
    // Détection du clic : relâchement après un appui
    if (!isPressed && wasPressed && clickTimer.getElapsedTime().asMilliseconds() > 100) {
        wasPressed = false;
        return true;
    }
    
    // Mise à jour de l'état
    if (isPressed && !wasPressed) {
        wasPressed = true;
        clickTimer.restart();
    }
    
    return false;
}

void Button::update() {
    // Réinitialisation si le clic est maintenu trop longtemps
    if (wasPressed && clickTimer.getElapsedTime().asMilliseconds() > 500) {
        wasPressed = false;
    }
}
void Button::setBackgroundColor(sf::RenderWindow& window) {
    if(!buttonTexture) {
        if(isMouseOver(window)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                shape.setFillColor(clickColor);
            } else {
                shape.setFillColor(hoverColor);
            }
        } else {
            shape.setFillColor(originalColor);
        }
    }
}

void Button::resetColor() {
    shape.setFillColor(originalColor);
}

void Button::drawPhoto(const sf::Texture& texture) {
    sf::Sprite sprite(texture);
    sprite.setPosition(shape.getPosition());
    sprite.setScale(shape.getSize().x / texture.getSize().x, shape.getSize().y / texture.getSize().y);
    shape.setTexture(&texture);
}
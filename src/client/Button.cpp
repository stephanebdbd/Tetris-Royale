// Button.cpp
#include "Button.hpp"
#include <iostream>


// Constructeur de la classe Rectangle
Rectangle::Rectangle(const sf::Vector2f& position, const sf::Vector2f& size, 
                     const sf::Color& FillColor, const sf::Color& outLineColor) {
    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(FillColor);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(outLineColor);
}

// Fonction pour dessiner le rectangle
void Rectangle::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

// Fonction pour dessiner une photo sur le rectangle
void Rectangle::drawPhoto(const sf::Texture& texture) {
    sf::Sprite sprite(texture);
    sprite.setPosition(shape.getPosition());
    // Conserver les proportions de l'image
    float scaleX = shape.getSize().x / texture.getSize().x;
    float scaleY = shape.getSize().y / texture.getSize().y;
    sprite.setScale(scaleX, scaleY);
    shape.setTexture(&texture);
}


// Constructeur de la classe Circle
Circle::Circle(const sf::Vector2f& position, const float& radius, 
               const sf::Color& FillColor, const sf::Color& outLineColor) {
    shape.setPosition(position);
    shape.setRadius(radius);
    shape.setFillColor(FillColor);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(outLineColor);
}

// Fonction pour dessiner le cercle
void Circle::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

// Fonction pour dessiner une photo sur le cercle
void Circle::drawPhoto(const sf::Texture& texture, sf::RenderWindow& window) {
    //sf::Texture texture;
    sf::Sprite sprite(texture);
    sprite.setPosition(shape.getPosition());
    // Conserver les proportions de l'image
    float scaleX = shape.getRadius() / texture.getSize().x;
    float scaleY = shape.getRadius() / texture.getSize().y;
    sprite.setScale(scaleX, scaleY);
    shape.setTexture(&texture);
    window.draw(sprite);
}


// Constructeur de la classe Button
Button::Button(const std::string& text, const sf::Font& font, unsigned int characterSize, 
               const sf::Color& textColor, const sf::Color& backgroundColor, 
               const sf::Vector2f& position, const sf::Vector2f& size, 
               const sf::Color& rectangleColor)
    {
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

// Fonction pour dessiner le bouton
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

// Fonction pour vérifier si la souris est au-dessus du bouton
bool Button::isMouseOver(const sf::RenderWindow& window) const {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    return shape.getGlobalBounds().contains(mousePos);
}

// Fonction pour vérifier si le bouton a été cliqué
bool Button::isClicked(sf::RenderWindow& window) {
    bool isPressed = isMouseOver(window) && sf::Mouse::isButtonPressed(sf::Mouse::Left);
    
    // Détection du clic : relâchement après un appui
    //std::cout << "isPressed: " << isPressed << ", wasPressed: " << wasPressed << std::endl;

    if (!isPressed && wasPressed && clickTimer.getElapsedTime().asMilliseconds() > 100) {
        //std::cout<< "Button clicked!" << std::endl;
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

// le client ne doit pas rester bloqué il doit attendre 0.5s avant de pouvoir recliquer
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

// Fonction pour réinitialiser la couleur du bouton
void Button::resetColor() {
    shape.setFillColor(originalColor);
}

// Fonction pour dessiner une photo sur le bouton
void Button::drawPhoto(const sf::Texture& texture) {
    sf::Sprite sprite(texture);
    sprite.setPosition(shape.getPosition());
    sprite.setScale(shape.getSize().x / texture.getSize().x, shape.getSize().y / texture.getSize().y);
    shape.setTexture(&texture);
}

// Fonction pour obtenir le texte du bouton
std::string Button::getText() const {
    return text.getString();
}
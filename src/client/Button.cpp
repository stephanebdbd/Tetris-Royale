#include "Button.hpp"
#include <iostream>
#include <algorithm>

sf::Sprite createScaledSprite(const sf::Texture& texture, const sf::Vector2f& targetSize, const sf::Vector2f& position) {
    sf::Sprite sprite(texture);
    float scaleX = targetSize.x / texture.getSize().x;
    float scaleY = targetSize.y / texture.getSize().y;
    sprite.setScale(scaleX, scaleY);
    sprite.setPosition(position);
    return sprite;
}


Rectangle::Rectangle(const sf::Vector2f& position, const sf::Vector2f& size, 
                     const sf::Color& fillColor, const sf::Color& outlineColor) {
    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(fillColor);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(outlineColor);
}

void Rectangle::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

void Rectangle::setTexture(const sf::Texture& texture) {
    shape.setTexture(&texture);
}

sf::Vector2f Rectangle::getPosition() const {
    return shape.getPosition();
}


Circle::Circle(const sf::Vector2f& position, float radius, 
               const sf::Color& fillColor, const sf::Color& outlineColor) {
    shape.setPosition(position);
    shape.setRadius(radius);
    shape.setFillColor(fillColor);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(outlineColor);
}

void Circle::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

void Circle::setTexture(const sf::Texture& texture) {
    shape.setTexture(&texture);
}


Button::Button(const std::string& text, const sf::Font& font, unsigned int characterSize, 
               const sf::Color& textColor, const sf::Color& backgroundColor, 
               const sf::Vector2f& position, const sf::Vector2f& size, 
               const sf::Color& outlineColor) {

    originalColor = backgroundColor;
    hoverColor = sf::Color(
        std::max(0, backgroundColor.r - 30),
        std::max(0, backgroundColor.g - 30),
        std::max(0, backgroundColor.b - 30),
        150);
    clickColor = sf::Color(
        std::max(0, backgroundColor.r - 50),
        std::max(0, backgroundColor.g - 50),
        std::max(0, backgroundColor.b - 50),
        150);

    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(backgroundColor);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(outlineColor);

    this->text.setFont(font);
    this->text.setString(text);
    this->text.setCharacterSize(characterSize);
    baseCharSize = characterSize;
    this->text.setFillColor(textColor);

    // Centrer le texte dans le bouton
    sf::FloatRect textRect = this->text.getLocalBounds();
    this->text.setOrigin(textRect.left + textRect.width / 2.0f, 
                         textRect.top + textRect.height / 2.0f);
    this->text.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
}

// Constructeur bouton image
Button::Button(const sf::Texture& texture, const sf::Vector2f& position, const sf::Vector2f& size) {
    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(sf::Color::White);
    buttonTexture = std::make_shared<sf::Texture>(texture);
}

void Button::setTexture(const sf::Texture& texture) {
    buttonTexture = std::make_shared<sf::Texture>(texture);
}

void Button::draw(sf::RenderWindow& window) const {
    if (buttonTexture) {
        sf::Sprite sprite = createScaledSprite(*buttonTexture, shape.getSize(), shape.getPosition());
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
    constexpr int CLICK_MIN_INTERVAL_MS = 100;

    bool isPressed = isMouseOver(window) && sf::Mouse::isButtonPressed(sf::Mouse::Left);

    if (!isPressed && wasPressed && clickTimer.getElapsedTime().asMilliseconds() > CLICK_MIN_INTERVAL_MS) {
        wasPressed = false;
        return true;
    }

    if (isPressed && !wasPressed) {
        wasPressed = true;
        clickTimer.restart();
    }

    return false;
}

void Button::update() {
    constexpr int CLICK_BLOCK_DURATION_MS = 500;
    if (wasPressed && clickTimer.getElapsedTime().asMilliseconds() > CLICK_BLOCK_DURATION_MS) {
        wasPressed = false;
    }
}

void Button::setBackgroundColor(sf::RenderWindow& window) {
    if (!buttonTexture) {
        if (isMouseOver(window)) {
            shape.setFillColor(sf::Mouse::isButtonPressed(sf::Mouse::Left) ? clickColor : hoverColor);
        } else {
            shape.setFillColor(originalColor);
        }
    }
}

void Button::resetColor() {
    shape.setFillColor(originalColor);
}

std::string Button::getText() const {
    return text.getString();
}

void Button::resize(float scaleX, float scaleY) {
    sf::Vector2f oldSize = shape.getSize();
    sf::Vector2f oldPosition = shape.getPosition();

    sf::Vector2f newSize = { oldSize.x * scaleX, oldSize.y * scaleY };
    sf::Vector2f newPosition = { oldPosition.x * scaleX, oldPosition.y * scaleY };

    shape.setSize(newSize);
    shape.setPosition(newPosition);

    unsigned int newCharSize = std::clamp(
        static_cast<unsigned int>(baseCharSize * std::min(scaleX, scaleY)),
        10u, 60u);
    text.setCharacterSize(newCharSize);

    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, 
                   textRect.top + textRect.height / 2.0f);
    text.setPosition(newPosition.x + newSize.x / 2.0f,
                     newPosition.y + newSize.y / 2.0f);
}

bool Button::operator==(const Button& other) const {
    return text.getString() == other.text.getString();
}

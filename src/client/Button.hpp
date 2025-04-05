// Button.hpp
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <memory>

class Rectangle {
    public:
        Rectangle(const sf::Vector2f& position, const sf::Vector2f& size, 
                const sf::Color& FillColor, const sf::Color& outLineColor);
        void draw(sf::RenderWindow& window) const;
        
    private:
        sf::RectangleShape shape;
};

class Circle {
    public:
        Circle(const sf::Vector2f& position, const sf::Vector2f& size, 
            const sf::Color& FillColor, const sf::Color& outLineColor);
        void draw(sf::RenderWindow& window);
        void setTexture(const std::shared_ptr<sf::Texture>& texture);
        
    private:
        sf::CircleShape shape;
        std::shared_ptr<sf::Texture> texture;
};

class Button {
    public:
        Button(const std::string& text, const sf::Font& font, unsigned int characterSize, 
            const sf::Color& textColor, const sf::Color& backgroundColor, 
            const sf::Vector2f& position, const sf::Vector2f& size, 
            const sf::Color& rectangleColor = sf::Color::Black);
        
        void draw(sf::RenderWindow& window) const;
        bool isMouseOver(const sf::RenderWindow& window) const;
        bool isClicked(sf::RenderWindow& window);
        void update();
        void setBackgroundColor(sf::RenderWindow& window);
        void resetColor();
        void drawPhoto(const sf::Texture& texture);
        
    private:
        sf::RectangleShape shape;
        sf::Text text;
        sf::Color backgroundColor;
        sf::Color textColor;
        sf::Color originalColor;
        sf::Color hoverColor;
        sf::Color clickColor;
        bool clicked;
        bool clickProcessed;
        bool wasPressed; // Nouvel état pour suivre l'état précédent
        sf::Clock clickTimer; // Pour gérer les délais entre les clics
        std::shared_ptr<sf::Texture> buttonTexture;
};
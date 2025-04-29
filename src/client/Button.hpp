// Button.hpp
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <memory>

class Rectangle {
    public:
        Rectangle(const sf::Vector2f& position, const sf::Vector2f& size, 
                const sf::Color& FillColor, const sf::Color& outLineColor);
        void draw(sf::RenderWindow& window) const;
        void drawPhoto(const sf::Texture& texture);
        sf::Vector2f getPosition() const ;
    private:
        sf::RectangleShape shape;
};

class Circle {
    public:
        Circle(const sf::Vector2f& position, const float& radius, 
            const sf::Color& FillColor, const sf::Color& outLineColor);
        void draw(sf::RenderWindow& window);
        void drawPhoto(const sf::Texture& texture, sf::RenderWindow& window);
        
    private:
        sf::CircleShape shape;
};

class Button {
    public:
        Button(const std::string& text, const sf::Font& font, unsigned int characterSize, 
            const sf::Color& textColor, const sf::Color& backgroundColor, 
            const sf::Vector2f& position, const sf::Vector2f& size, 
            const sf::Color& rectangleColor = sf::Color::Black);
        Button(const sf::Texture& texture, const sf::Vector2f& position, const sf::Vector2f& size);
        
        void draw(sf::RenderWindow& window) const;
        bool isMouseOver(const sf::RenderWindow& window) const;
        bool isClicked(sf::RenderWindow& window);
        void update();
        void setBackgroundColor(sf::RenderWindow& window);
        void resetColor();
        //void drawPhoto(const sf::Texture& texture);
        std::string getText() const;
        void resize(float scaleX, float scaleY) ;

        
        bool operator==(const Button& other) const {
            return text.getString() == other.text.getString();
        }
        void setTexture(const sf::Texture& texture);
        
    private:
        sf::RectangleShape shape;
        sf::Text text;
        sf::Color backgroundColor;
        sf::Color textColor;
        sf::Color originalColor;
        sf::Color hoverColor;
        sf::Color clickColor;
        bool clicked = false ; // État du bouton
        bool wasPressed = false ; // Nouvel état pour suivre l'état précédent
        sf::Clock clickTimer; // Pour gérer les délais entre les clics
        std::shared_ptr<sf::Texture> buttonTexture; // Texture pour le bouton (optionnel)
        unsigned int baseCharSize;
};
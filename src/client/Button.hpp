#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <functional>

// Classe pour les boutons
class Button {
    public:
        Button(const std::string& text, const sf::Font& font, unsigned int characterSize, 
               const sf::Color& textColor, const sf::Color& backgroundColor, 
               const sf::Vector2f& position, const sf::Vector2f& size);
    
        void draw(sf::RenderWindow& window) const;
        bool isMouseOver(const sf::RenderWindow& window) const;
        bool isMousePressed(const sf::RenderWindow& window) const;
        void setBackgroundColor();
        void resetColor();
    
    private:
        sf::RectangleShape shape;
        sf::Text text;
        sf::Color backgroundColor;
        sf::Color textColor;
        sf::Color originalColor;
        sf::Color hoverColor;
        sf::Color clickColor;
        std::function<void()> onClickCallback;
    
    };
    
#include <SFML/Graphics.hpp>


// Classe pour les champs de texte
class TextField {
    public:
        TextField(const sf::Font& font, unsigned int characterSize, 
                  const sf::Color& textColor, const sf::Color& backgroundColor, 
                  const sf::Vector2f& position, const sf::Vector2f& size, 
                  const std::string& placeholder, bool hidden = false);
    

        void draw(sf::RenderWindow& window) const;
        bool isMouseOver(const sf::RenderWindow& window) const;
        void setActive(bool active);
        void handleInput(sf::Event event);
        std::string getText() const;
        void updateCursor();
        void validateInput();
        void clear();
    
    private:
        sf::RectangleShape shape;
        sf::Text text;
        sf::Text placeholderText;
        std::string textString;
        std::string placeholder;
        bool isActive;
        unsigned int maxLength;
        bool showCursor;
        sf::Clock cursorClock;
        sf::RectangleShape cursor;
        float padding;
        bool hiddenText = false;
};
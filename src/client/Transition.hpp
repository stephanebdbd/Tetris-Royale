#ifndef TRANSITION_HPP
#define TRANSITION_HPP
#include <SFML/Graphics.hpp>


// Interface de base pour les transitions
class BaseTransition {
    public:
        virtual ~BaseTransition() = default;
        virtual void start(sf::RenderWindow& window) = 0;
        virtual bool update(float deltaTime) = 0;
        virtual void draw(sf::RenderWindow& window) = 0;
        virtual bool isComplete() const = 0;
    };
    
    // Transition par fondu
    class FadeTransition : public BaseTransition {
        sf::RectangleShape overlay;
        float duration;
        float progress;
        bool fadingOut;
        bool active;
    public:
        FadeTransition(sf::Vector2f size, float dur = 0.5f);
        void start(sf::RenderWindow& window) override;
        bool update(float deltaTime) override;
        void draw(sf::RenderWindow& window) override;
        bool isComplete() const override;
    };

#endif
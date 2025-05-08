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
        FadeTransition(sf::Vector2f size, float dur = 0.5f) 
            : overlay(size), duration(dur), progress(0), fadingOut(true), active(false) {
            overlay.setFillColor(sf::Color::Black);
            overlay.setPosition(0, 0);
        }
    
        void start(sf::RenderWindow& window) override {
            progress = 0;
            fadingOut = true;
            active = true;
            overlay.setFillColor(sf::Color::Black);
        }
    
        bool update(float deltaTime) override {
            if (!active) return false;
    
            progress += deltaTime / duration;
            if (progress >= 1.0f) {
                progress = 1.0f;
                if (fadingOut) {
                    fadingOut = false;
                    progress = 0;
                    return false;
                } else {
                    active = false;
                    return true;
                }
            }
    
            float alpha = fadingOut ? progress * 255 : (1 - progress) * 255;
            overlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(alpha)));
            return false;
        }
    
        void draw(sf::RenderWindow& window) override {
            if (active) {
                window.draw(overlay);
            }
        }
    
        bool isComplete() const override { return !active && !fadingOut; }
    };

#endif // TRANSITION_HPP
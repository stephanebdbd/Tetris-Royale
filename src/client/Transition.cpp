#include "Transition.hpp"

FadeTransition::FadeTransition(sf::Vector2f size, float dur = 0.5f) 
    : overlay(size), duration(dur), progress(0), fadingOut(true), active(false) {
    overlay.setFillColor(sf::Color::Black);
    overlay.setPosition(0, 0);
}

void FadeTransition::start(sf::RenderWindow& window) {
    progress = 0;
    fadingOut = true;
    active = true;
    overlay.setFillColor(sf::Color::Black);
}

bool FadeTransition::update(float deltaTime) {
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

void FadeTransition::draw(sf::RenderWindow& window)  {
    if (active) {
        window.draw(overlay);
    }
}

bool FadeTransition::isComplete() const { return !active && !fadingOut; }
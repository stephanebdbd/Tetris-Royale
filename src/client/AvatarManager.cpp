#include "AvatarManager.hpp"
#include <iostream>

AvatarManager::AvatarManager(sf::RenderWindow* window)
    : window(window), selectedAvatar(-1) {}

void AvatarManager::drawAvatar(int avatarIndex, float posX, float posY, float size) {
    if (avatarIndex >= 0 && avatarIndex < static_cast<int>(avatarPaths.size())) {
        sf::Texture avatarTexture;
        if (avatarTexture.loadFromFile(avatarPaths[avatarIndex])) {
            sf::Sprite avatarSprite;
            avatarSprite.setTexture(avatarTexture);

            // Redimensionner l'avatar pour qu'il rentre bien dans le bouton
            avatarSprite.setScale(
                size / avatarTexture.getSize().x,
                size / avatarTexture.getSize().y
            );

            // Positionner l'avatar
            avatarSprite.setPosition(posX, posY);

            window->draw(avatarSprite);
        } else {
            std::cout << "Erreur de chargement de l'avatar !" << std::endl;
        }
    }
}

int AvatarManager::getSelectedAvatar() const {
    return selectedAvatar;
}

std::vector<std::string> AvatarManager::getAvatarPath() const {
    return avatarPaths;
}

void AvatarManager::loadAvatarPaths(const std::vector<std::string>& paths) {
    avatarPaths = paths;
}

int AvatarManager::getAvatarClickedContact() const {
    return avatarClickedContact;
}

sf::Texture* AvatarManager::getAvatarTexture(int index) {
    if (index >= 0 && index < static_cast<int>(avatarPaths.size())) {
        auto* texture = new sf::Texture();
        if (texture->loadFromFile(avatarPaths[index])) {
            return texture;
        }
        delete texture; // important : éviter fuite mémoire si load échoue
    }
    return nullptr;
}

void AvatarManager::setAvatarClickedContact(int index) {
    avatarClickedContact = index;
}

void AvatarManager::setSelectedAvatar(int index) {
    if (index >= 0 && index < static_cast<int>(avatarPaths.size())) {
        selectedAvatar = index;
    }
}


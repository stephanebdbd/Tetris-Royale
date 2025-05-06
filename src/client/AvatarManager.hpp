#ifndef AVATAR_MANAGER_HPP
#define AVATAR_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>

class AvatarManager {

    sf::RenderWindow* window;
    std::vector<std::string> avatarPaths;
    int selectedAvatar = -1;
    int avatarClickedContact;

public:
    AvatarManager(sf::RenderWindow* window);

    void loadAvatarPaths(const std::vector<std::string>& paths);
    void drawAvatar(int avatarIndex, float posX, float posY, float size);


    int getSelectedAvatar() const;
    int getAvatarClickedContact() const;
    std::vector<std::string> getAvatarPath() const;
    sf::Texture* getAvatarTexture(int index);
    
    void setSelectedAvatar(int index);
    void setAvatarClickedContact(int index);
};

#endif 

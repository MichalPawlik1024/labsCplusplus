//
// Created by Michał on 01.06.2025.
//

#ifndef EASIERDESIGN_H
#define EASIERDESIGN_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>

class DraggableObject {
public:
    DraggableObject(const sf::Texture& texture, const sf::Vector2f& position) {
        sprite.setTexture(texture);
        sprite.setPosition(position);
    }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {

            if (sprite.getGlobalBounds().contains((sf::Vector2f)sf::Mouse::getPosition(window))) {
                dragging = true;
                offset = sprite.getPosition() - (sf::Vector2f)sf::Mouse::getPosition(window);
            }
            }

        if (event.type == sf::Event::MouseButtonReleased &&
            event.mouseButton.button == sf::Mouse::Left) {
            dragging = false;
            }
    }

    void update(const sf::RenderWindow& window) {
        if (dragging) {
            sprite.setPosition((sf::Vector2f)sf::Mouse::getPosition(window) + offset);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    void savePosition(nlohmann::json & data) {
            data["x"]=sprite.getPosition().x;
            data["y"]=sprite.getPosition().y;
    }

    void loadPosition(const std::string& filename) {
        std::ifstream file(filename);
        float x, y;
        if (file >> x >> y) {
            sprite.setPosition(x, y);
        }
    }
    virtual void flip() {
        const sf::Texture * texture= sprite.getTexture();
        sf::Image image = texture->copyToImage();

        unsigned int width = image.getSize().x;
        unsigned int height = image.getSize().y;

        sf::Image flippedImage;
        flippedImage.create(width, height);

        for (unsigned int y = 0; y < height; ++y) {
            for (unsigned int x = 0; x < width; ++x) {
                flippedImage.setPixel(width - x - 1, y, image.getPixel(x, y));
            }
        }
        this->texture=new sf::Texture();
        this->texture->loadFromImage(flippedImage);
        sprite.setTexture(*this->texture);

    }


    sf::Sprite sprite;
    bool dragging = false;
    sf::Vector2f offset;
    sf::Texture * texture;
};



#endif //EASIERDESIGN_H

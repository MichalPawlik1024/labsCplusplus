//
// Created by Michał on 18.04.2025.
//

#ifndef BASICCLASSES_H
#define BASICCLASSES_H
#include <SFML/Graphics/Sprite.hpp>

//#include "CrucialData.h"

enum class WidgetType{basic, basicGraphical,
    button, textInput, player, dice, weapon, texturedButton };



class BasicWidget {
public:
    BasicWidget() {

    }
    virtual void draw(sf::RenderWindow * master){}
    virtual WidgetType getWidgetType()  {
        return WidgetType::basic;
    }
    virtual ~BasicWidget() {}
};

class GraphicalObject : public BasicWidget {

    sf::Sprite  sprite;
    sf::Texture texture;
public:
    GraphicalObject(std::string texturePath, float posX, float posY):BasicWidget() {
        sf::Image image;
        texture.loadFromFile(texturePath);
        sprite.setTexture(texture);
        sprite.setPosition(posX, posY);
    }
    void draw(sf::RenderWindow * master) override {
        master->draw(sprite);
    }
    WidgetType getWidgetType() override {
        return WidgetType::basicGraphical;
    }
    bool isClicked(sf::RenderWindow * master) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*master);
        sf::FloatRect bounds = sprite.getGlobalBounds();
        bool inside = bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        bool clicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        if (inside && clicked) {
            return true;
        }
        return false;
    }
   virtual  void setTexture(sf::Texture & texture) {
        this->texture = texture;
        sprite.setTexture(this->texture);
    }
    virtual void setPosition(float posX, float posY) {
        sprite.setPosition(posX, posY);
    }
     virtual void setMask(sf::Color color) {
        sf::Image img=texture.copyToImage();

        img.createMaskFromColor(color);
        for (int i=0;i<img.getSize().x;i++) {
            for (int j=0;j<img.getSize().y;j++) {
                if (img.getPixel(i,j)==color) {
                    img.setPixel(i,j,sf::Color::Black);

                }
            }
        }
        texture.loadFromImage(img);

    }
    virtual sf::Vector2f getPosition() {
        return sprite.getPosition();
    }
    virtual void rotate(float rotationAngle) {
        this->sprite.rotate(rotationAngle);
    }
    virtual sf::Vector2u getSize() {
        return texture.getSize();
    }
    virtual void flip() {
        sf::Image image = texture.copyToImage();

        unsigned int width = image.getSize().x;
        unsigned int height = image.getSize().y;

        sf::Image flippedImage;
        flippedImage.create(width, height);

        for (unsigned int y = 0; y < height; ++y) {
            for (unsigned int x = 0; x < width; ++x) {
                flippedImage.setPixel(width - x - 1, y, image.getPixel(x, y));
            }
        }

        texture.loadFromImage(flippedImage);
        sprite.setTexture(texture);

    }
    void setRotation(float angle) {
        sprite.setRotation(angle);
    }
    virtual ~GraphicalObject() {}
};






#endif //BASICCLASSES_H

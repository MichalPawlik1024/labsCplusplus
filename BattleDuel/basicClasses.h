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
        sprite.setTexture(texture);
    }
    void setPosition(float posX, float posY) {
        sprite.setPosition(posX, posY);
    }

    virtual ~GraphicalObject() {}
};






#endif //BASICCLASSES_H

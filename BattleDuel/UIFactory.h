//
// Created by Michał on 27.05.2025.
//

#ifndef UIFACTORY_H
#define UIFACTORY_H
#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <nlohmann/json.hpp> // lub inna biblioteka do JSON
#include "GameMechanicWidgets.h"
#include"Game.h"

enum class ObjectType {
    Button,
    DefaultButton,
    Text,
    HealthBar,
    Dice
};

class UIFactory {
public:
    UIFactory(sf::Vector2u baseResolution, sf::Vector2u actualResolution);

    void loadFromJson( nlohmann::json& jsonConfig);
    template<typename T>
        T at(std::string id);
    std::map<std::string, DefaultButton*> defaultButtons;
    std::map<std::string, Button*> buttons;
    std::map<std::string, TextDisplayer*> texts;
    std::map<std::string, HealthBar*> healthBars;
    std::map<std::string, Dice*> dices;

private:
    sf::Vector2f scaleFactor;
    void scalePosition( nlohmann::json& original);
    void scaleSize( nlohmann::json& original);

    void createObject( nlohmann::json& obj);
    ObjectType resolveType( std::string typeStr);


};
//
// Created by Michał on 27.05.2025.
//
template<>
TextDisplayer* UIFactory::at(std::string id) {
    return texts.at(id);
}

UIFactory::UIFactory(sf::Vector2u baseRes, sf::Vector2u actualRes) {
    scaleFactor = {
        static_cast<float>(actualRes.x) / baseRes.x,
        static_cast<float>(actualRes.y) / baseRes.y
    };
}

void UIFactory::scalePosition(nlohmann::json & original) {
     original["position"][0] =static_cast<float>(original["position"][0])*scaleFactor.x;
     original["position"][1] = static_cast<float>(original["position"][1])*scaleFactor.y ;
}

void UIFactory::scaleSize(nlohmann::json & original) {
    original["size"][0] =static_cast<float>(original["size"][0])*scaleFactor.x;
    original["size"][1] = static_cast<float>(original["size"][1])*scaleFactor.y ;
}

ObjectType UIFactory::resolveType( std::string typeStr) {
    if (typeStr == "button") return ObjectType::Button;
    if (typeStr == "defaultButton") return ObjectType::DefaultButton;
    if (typeStr == "text") return ObjectType::Text;
    if (typeStr == "healthbar") return ObjectType::HealthBar;
    if (typeStr == "dice") return ObjectType::Dice;
    throw std::runtime_error("Unknown object type: " + typeStr);
}

void UIFactory::loadFromJson( nlohmann::json& jsonConfig) {
    for ( auto& element : jsonConfig) {
        createObject(element);
    }
}

void UIFactory::createObject( nlohmann::json& obj) {
    std::string id = obj.at("id");
    ObjectType type = resolveType(obj.at("type"));
    scalePosition(obj);
    scaleSize(obj);
    switch (type) {
        case ObjectType::Button: {
            //Button* button = new Button(obj); // dostosuj konstruktor
            //buttons[id] = button;
            break;
        }
        case ObjectType::DefaultButton: {
           // DefaultButton* button = new DefaultButton(obj); // dostosuj konstruktor
          //  defaultButtons[id] = button;
            break;
        }
        case ObjectType::Text: {
            TextDisplayer* text = new TextDisplayer(1,obj); // dostosuj konstruktor
            texts[id] = text;
            break;
        }
        case ObjectType::HealthBar: {
            //HealthBar* hb = new HealthBar(obj); // dostosuj konstruktor
            //healthBars[id] = hb;
            break;
        }
        case ObjectType::Dice: {
         //   Dice* dice = new Dice(obj); // dostosuj konstruktor
          //  dices[id] = dice;
            break;
        }
    }
}


class JsonUILoader {
    public:

    static std::vector<DefaultButton*> loadFromJson(std::string fileDir,sf::Color backColor,sf::Color fontColor) {
        std::vector<DefaultButton*> buttons;
        std::ifstream jsonFile(fileDir);
        nlohmann::json data=nlohmann::json::parse(jsonFile);
        for (auto& singleElement:data) {
            auto newBtn=new DefaultButton(singleElement["text"],singleElement["size"][0],singleElement["size"][1],
                singleElement["position"][0],singleElement["position"][1],backColor,fontColor);
            buttons.push_back(newBtn);
        }
        return buttons;
    }
    static std::vector<TextDisplayer*> loadTextDisplayersFromJson(const std::string fileDir, const std::string fontFile, unsigned int size = 30, sf::Color color = sf::Color::White) {
        std::ifstream jsonFile(fileDir);
        nlohmann::json data = nlohmann::json::parse(jsonFile);

        std::vector<TextDisplayer*> texts;
        for (auto& element : data) {
            auto displayer = new TextDisplayer(
                fontFile,
                element.value("text", ""),
                size,
                color,
                element["position"][0],
                element["position"][1]
            );
            texts.push_back(displayer);
        }
        return texts;
    }


};




#endif //UIFACTORY_H

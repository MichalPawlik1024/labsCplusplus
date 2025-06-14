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



class JsonUILoader {
    public:

    static std::vector<DefaultButton*> loadFromJson(std::string fileDir,sf::Color backColor,sf::Color fontColor) {
        std::vector<DefaultButton*> buttons;
        std::ifstream jsonFile(fileDir);
        nlohmann::json data=nlohmann::json::parse(jsonFile);
        for (auto& singleElement:data) {
            auto newBtn=new DefaultButton(singleElement["text"],singleElement["size"][0],singleElement["size"][1],
                singleElement["position"][0],singleElement["position"][1],50,backColor,fontColor);
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

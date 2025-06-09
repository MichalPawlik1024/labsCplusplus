//
// Created by Michał on 17.03.2025.
//

#ifndef WINDOWMONITOR_H
#define WINDOWMONITOR_H


class TextInput: public BasicWidget {
private:
    sf::RectangleShape box;
    sf::Text text;
    sf::Font font;
    std::string input;
    bool isActive = false;
    sf::RenderWindow * master;
public:
    TextInput(float x, float y, float width, float height,sf::RenderWindow * master);

    void handleEvent(sf::Event & event);

    void draw();

    std::string getText() const;

    void clearText();
};

inline TextInput::TextInput(float x, float y, float width, float height,sf::RenderWindow * master):master(master), BasicWidget() {
    // Load a font
    if (!font.loadFromFile("Assets/Minecraft/minecraft.ttf")) {
        std::cerr << "Failed to load font\n";
    }
    text.setFont(font);
    text.setCharacterSize(height-70);

    text.setFillColor(sf::Color::Green);
    text.setPosition(x + 5, y + 5);
    box.setSize(sf::Vector2f(width, height));
    box.setPosition(x, y);
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color::White);
    box.setFillColor(sf::Color::Transparent);
}

inline void TextInput::handleEvent(sf::Event &event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        isActive = box.getGlobalBounds().contains(mousePos);
        box.setOutlineColor(isActive ? sf::Color::Green : sf::Color::White);
    }
    if (isActive && event.type == sf::Event::TextEntered) {
        if (event.text.unicode == '\b' && !input.empty()) {
            input.pop_back();  // Handle backspace
        } else if (event.text.unicode < 128 && event.text.unicode != '\b') {
            input += static_cast<char>(event.text.unicode);  // Add character
        }
        text.setString(input);
    }
}

inline void TextInput::draw() {
    master->draw(box);
    master->draw(text);
}

inline std::string TextInput::getText() const {
    return input;
}

inline void TextInput::clearText() {
    input.clear();
    text.setString("");
}

#endif //WINDOWMONITOR_H

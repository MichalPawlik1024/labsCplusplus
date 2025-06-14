//
// Created by Michał on 18.04.2025.
//

#ifndef CrucialData_H
#define CrucialData_H
#include <array>

Button* loadButton(const nlohmann::json& json) {
    std::string texture = json["texture"];
    float x = json["x"];
    float y = json["y"];
    bool visible = json.value("visible", true);
    return new Button(texture, x, y, visible);
}
TextDisplayer* loadTextDisplayer(const nlohmann::json& json) {
    std::string font = json["font"];
    std::string text = json["text"];
    int size = json["size"];
    auto colorArr = json["color"];
    sf::Color color(colorArr[0], colorArr[1], colorArr[2]);
    float x = json["x"];
    float y = json["y"];
    return new TextDisplayer(font, text, size, color, x, y);
}
Dice* loadDice(const nlohmann::json& json) {
    float x = json["x"];
    float y = json["y"];
    return new Dice(x, y);
}
Scene* loadScene(const nlohmann::json& json) {
    std::string background = json["background"];
    return new Scene(background);
}
void loadWeaponPositions(const nlohmann::json& json, Weapon * weapon) {
    //std::string type = json["type"];
    float x = json["x"];
    float y = json["y"];
    weapon->setPosition(x, y);
}

HealthBar* loadHealthBar(const nlohmann::json& json) {
    float x = json["x"];
    float y = json["y"];
    float sizeY = json["sizeY"];
    float sizeX = json["sizeX"];
    return new HealthBar(sf::Vector2f(sizeX,sizeY),sf::Vector2f(x,y),100);
}





class GameWidgets {
public:
    enum class SceneType{mainMenu,duelScene,upgradeScene, newGame};
    ///To reduce size of code , reduce
    enum class TextInfoType{playerName,opponentName,playerHealth,opponentHealth};
    static std::vector<sf::Texture*> diceTextures;
    static sf::RenderWindow * master;
    ///TODO upgrade window static , new game window be volatile
    static Button * attackButton;
    static Button * newGame;
    static Button * loadGame;
    static Button * upgrade;
    static Button * quit;
    static Button * saveAndExit;
    static Player * player;
    static Dice * playerDice; static Dice * opponentDice;
    static Fighter * oponent;
    static std::unordered_map<SceneType,Scene*> scenes;
    static HealthBar * playerHealthBar;
    static HealthBar * opponentHealthBar;
    ///TODO pack it into map or vector
    static TextDisplayer * playerNameText;
    static TextDisplayer * opponentNameText;
    static TextDisplayer * playerHealthInfo;
    static TextDisplayer * opponentHealthInfo;
    static TextDisplayer * xpInfo;
    static TextDisplayer * moneyInfo;
    static FigthResultDisplayer * resultDisplayer;
    static TextInput * newGameInput;

    static Weapon * playerWeapon;
    static Weapon * oponentWeapon;
    static std::vector<BasicWidget*> mainMenu;
    static std::vector<BasicWidget*> duelScene;
    static std::vector<BasicWidget*> staticTexts;
    static TextDisplayer * coolDown;
    static void drawScene(SceneType & sceneType) {
        switch (sceneType) {
            case SceneType::mainMenu:
                scenes[sceneType]->draw(master);
                std::for_each(mainMenu.begin(),mainMenu.end(),[](auto & B) {B->draw(master);});
                break;
            case SceneType::duelScene:
                scenes[sceneType]->draw(master);
                attackButton->draw(master);
                playerDice->draw(master);
                opponentDice->draw(master);
                playerNameText->draw(master);
                opponentNameText->draw(master);
                resultDisplayer->draw(master);
                playerHealthBar->draw(master);
                opponentHealthBar->draw(master);
                playerHealthInfo->draw(master);
                opponentHealthInfo->draw(master);
                moneyInfo->draw(master);
                xpInfo->draw(master);
                playerWeapon->draw(master);
                oponentWeapon->draw(master);
                coolDown->draw(master);
               // player->draw(master);
                oponent->draw(master);
                std::for_each(staticTexts.begin(),staticTexts.end(),[&](auto & A) {
                    A->draw(master);
                });
                break;
            case SceneType::newGame:
                newGameInput->draw();
                break;
        }
    }
    static void release() {
        delete scenes[SceneType::upgradeScene];
        delete scenes[SceneType::mainMenu];
        delete attackButton;
        delete newGame;
        delete loadGame;
        delete saveAndExit;
        delete playerDice;
        delete opponentDice;
        delete scenes[SceneType::duelScene];
        delete playerHealthBar;
        delete opponentHealthBar;
        delete playerHealthInfo;
        delete opponentHealthInfo;
        delete xpInfo;
        delete moneyInfo;
        delete resultDisplayer;
        delete newGameInput;
        delete playerWeapon;
        delete playerNameText;
        delete opponentNameText;
        delete oponentWeapon;
        for (auto & texture: diceTextures) {
            delete texture;
        }
        for (auto & staticText:staticTexts)
            delete staticText;
       Logger::writeMessage("Everything deallocated safely\n");
    }
    static void load();
    static void loadUIFromJson(const std::string & path);
};


std::vector<sf::Texture*> GameWidgets::diceTextures = {};
Button * GameWidgets::attackButton;
Button * GameWidgets::newGame;
Button * GameWidgets::loadGame;
Button * GameWidgets::upgrade;
Button * GameWidgets::quit;
Button * GameWidgets::saveAndExit;
//OldPlayer * GameWidgets::player;
//OldPlayer * GameWidgets::oponent;
Player * GameWidgets::player;
Fighter * GameWidgets::oponent;


std::unordered_map<GameWidgets::SceneType,Scene*> GameWidgets::scenes;
Dice * GameWidgets::opponentDice;
Dice * GameWidgets::playerDice;
HealthBar * GameWidgets::playerHealthBar;
HealthBar * GameWidgets::opponentHealthBar;
TextDisplayer * GameWidgets::coolDown;
TextDisplayer * GameWidgets::opponentNameText;
TextDisplayer * GameWidgets::playerNameText;
FigthResultDisplayer * GameWidgets::resultDisplayer;
TextInput * GameWidgets::newGameInput;
TextDisplayer * GameWidgets::playerHealthInfo;
TextDisplayer * GameWidgets::opponentHealthInfo;
TextDisplayer * GameWidgets::moneyInfo;
TextDisplayer * GameWidgets::xpInfo;
sf::RenderWindow * GameWidgets::master;
Weapon * GameWidgets::playerWeapon;
Weapon * GameWidgets::oponentWeapon;

std::vector<BasicWidget *> GameWidgets::duelScene{};
std::vector<BasicWidget *> GameWidgets::mainMenu{};
std::vector<BasicWidget *> GameWidgets::staticTexts;

void GameWidgets::load() {
    try {
        for (int i=1;i<7;i++) {
            std::string path="Assets/Dice";
            path+=std::to_string(i);
            path+=".jpg";
            try {
                diceTextures.emplace_back(new sf::Texture());
                diceTextures.at(i-1)->loadFromFile(path);
            }
            catch (std::exception & e) {
                std::cerr << e.what() << std::endl;
            }
        }
    }
    catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    }
    playerWeapon=new ShotGun();
    oponentWeapon=new ShotGun();
    oponentWeapon->setPosition(1200.0,800.0);
    const std::string minecraftFont= "Assets/minecraft/Minecraft.ttf";
    GameWidgets::attackButton=new Button("Assets/AttackButton.jpg",600.0f,100.0f,false);
    GameWidgets::scenes[GameWidgets::SceneType::upgradeScene] = new Scene("Assets/UpgradingStatsWindow.jpg");
    GameWidgets::scenes[GameWidgets::SceneType::mainMenu]=new Scene("Assets/mainMenu.jpg");
    GameWidgets::newGame=new Button("Assets/newGameButton.jpg",400.0f ,200.0f,true);
    GameWidgets::loadGame=new Button("Assets/loadGameButton.jpg",400.0f,300.0f,true);
    GameWidgets::saveAndExit= new Button("Assets/saveAndExitButton.jpg",400.0f,400.0f,true);
    GameWidgets::scenes[GameWidgets::SceneType::duelScene]=new Scene("Assets/duelScene.jpg");
    GameWidgets::playerDice= new Dice(200.0f,400.0f);
    GameWidgets::opponentDice= new Dice(600.0f,400.0f);
    GameWidgets::playerNameText = new TextDisplayer(minecraftFont,"Nibemben",
        30,sf::Color::White,200.0,350.0);
    GameWidgets::opponentNameText=new TextDisplayer(minecraftFont,"Major",30,sf::Color::White,
        600.0,350.0);
    GameWidgets::resultDisplayer=new FigthResultDisplayer();
    GameWidgets::moneyInfo=new TextDisplayer(minecraftFont);
    GameWidgets::xpInfo=new TextDisplayer(minecraftFont);
    GameWidgets::mainMenu={loadGame,newGame,saveAndExit};

}

inline void GameWidgets::loadUIFromJson(const std::string &path)  {

///Dice
///Textures
    try {
        for (int i=1;i<7;i++) {
            std::string path="Assets/Dice";
            path+=std::to_string(i);
            path+=".jpg";
            try {
                diceTextures.emplace_back(new sf::Texture());
                diceTextures.at(i-1)->loadFromFile(path);
            }
            catch (std::exception & e) {
                std::cerr << e.what() << std::endl;
            }
        }

        for (int i=0; i<  OpponentUpgradeCenter::charactersTexturePaths.size(); i++) {
            std::cout << OpponentUpgradeCenter::charactersTexturePaths[i] << std::endl;
            OpponentUpgradeCenter::charactersTextures.emplace_back( new sf::Texture());
            OpponentUpgradeCenter::charactersTextures.at(i)->loadFromFile(OpponentUpgradeCenter::charactersTexturePaths.at(i));

        }


    }
    catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    }



    std::ifstream f(path);
    nlohmann::json data = nlohmann::json::parse(f);

    // SCENE BACKGROUNDS
    GameWidgets::scenes[GameWidgets::SceneType::duelScene] = loadScene(data["duelScene"]);
    GameWidgets::scenes[GameWidgets::SceneType::mainMenu] = loadScene(data["mainMenu"]);

    // BUTTONS
    GameWidgets::attackButton = loadButton(data["duelScene"]["buttons"]["attack"]);
    GameWidgets::newGame = loadButton(data["mainMenu"]["buttons"]["newGame"]);
    GameWidgets::loadGame = loadButton(data["mainMenu"]["buttons"]["loadGame"]);
    GameWidgets::saveAndExit = loadButton(data["mainMenu"]["buttons"]["saveAndExit"]);

    // DICE
    GameWidgets::playerDice = loadDice(data["duelScene"]["dice"]["player"]);
    GameWidgets::opponentDice = loadDice(data["duelScene"]["dice"]["opponent"]);

    // TEXT
    GameWidgets::playerNameText = loadTextDisplayer(data["duelScene"]["texts"]["playerName"]);
    GameWidgets::opponentNameText = loadTextDisplayer(data["duelScene"]["texts"]["opponentName"]);
    GameWidgets::xpInfo=loadTextDisplayer(data["duelScene"]["texts"]["xpInfo"]);
    GameWidgets::moneyInfo=loadTextDisplayer(data["duelScene"]["texts"]["moneyInfo"]);
    GameWidgets::playerHealthInfo=loadTextDisplayer(data["duelScene"]["texts"]["playerHealthInfo"]);
    GameWidgets::opponentHealthInfo=loadTextDisplayer(data["duelScene"]["texts"]["opponentHealthInfo"]);
    GameWidgets::staticTexts.emplace_back(loadTextDisplayer(data["duelScene"]["texts"]["yourWeapon"]));
    GameWidgets::staticTexts.emplace_back(loadTextDisplayer(data["duelScene"]["texts"]["opponentWeapon"]));
    GameWidgets::resultDisplayer=new FigthResultDisplayer();
    GameWidgets::coolDown=loadTextDisplayer(data["duelScene"]["texts"]["coolDown"]);

    // Bars
    GameWidgets::playerHealthBar=loadHealthBar(data["duelScene"]["healthBars"]["player"]);
    GameWidgets::opponentHealthBar=loadHealthBar(data["duelScene"]["healthBars"]["opponent"]);

    // WEAPONS
    GameWidgets::playerWeapon = new ShotGun();
    GameWidgets::oponentWeapon = new ShotGun();

    // MAIN MENU LIST
    GameWidgets::mainMenu = {GameWidgets::loadGame, GameWidgets::newGame, GameWidgets::saveAndExit};
}





#endif //CRUCIALDATA_H


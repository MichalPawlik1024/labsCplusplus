//
// Created by Michał on 18.04.2025.
//

#ifndef CrucialData_H
#define CrucialData_H
#include <array>


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
    static Button * saveAndExit; ///TODO odmateuszowic kod
    //static OldPlayer* player;
    static Player * player;

    static Dice * playerDice; static Dice * opponentDice;
    //static OldPlayer* oponent;
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

    static void drawScene(SceneType & sceneType) {
#ifdef InGame
        scenes[sceneType]->draw();
#endif
        switch (sceneType) {
            case SceneType::mainMenu:
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
       Logger::writeMessage("Everything deallocated safely\n");
    }
    static void load();

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
    oponentWeapon=new ShotGun(); oponentWeapon->setPosition(1200.0,800.0);
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





#endif //CRUCIALDATA_H


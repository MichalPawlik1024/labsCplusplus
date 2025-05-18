//
// Created by Michał on 21.03.2025.
//

#ifndef GAME_H
#define GAME_H
#include "CrucialData.h"
//#include "GameMechanicWidgets.h"
//#include "CrucialData.h"

class CompetingForAttack {
public:
    enum class WhoAttacked { player, opponent,undecided };
    std::atomic<WhoAttacked> attacker;
    std::condition_variable playerCV;
    std::condition_variable tura;
    std::mutex playerMtx;
    std::mutex turaMtx;
    std::mutex opponentMutex;
    std::atomic<bool> playerAttacked{false};
    std::atomic<bool> opponentAttacked{false};
    std::atomic<bool> decisionLocked{false};
    std::atomic<bool> playerPressed{false};
    std::atomic<bool> active{false};
    std::atomic<bool> running{true};
    std::atomic<bool> roudEnded{false};

    CompetingForAttack() = default;

    void opponentTriesToAttack() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1500)); // Cooldown przeciwnika
        {
            bool expected=false;
            opponentMutex.lock();
            if (decisionLocked.compare_exchange_strong(expected,true)) {
                playerPressed.store(false);
                attacker.store(WhoAttacked::opponent);
            }
            opponentMutex.unlock();
        }
        playerCV.notify_all();
    }

    void playerTriesToAttack() {
        std::unique_lock<std::mutex> lock(playerMtx);
        ///We won't wait eternity for player to press key
        playerCV.wait(lock, [this]() { return playerPressed.load() || !running.load()|| decisionLocked.load(); });
        if (!running) return;
        {
            bool expected=false;
            ///we jummp into decision
            if (decisionLocked.compare_exchange_strong(expected,true)) {
                opponentMutex.lock(); //no place for opponent to come in with his decision
                playerAttacked = true;
                attacker=WhoAttacked::player;
                opponentMutex.unlock();
            }
        }
    }

    void main() {
        while (running.load()) {
            playerAttacked = false;
            opponentAttacked = false;
            decisionLocked = false;
            playerPressed = false;
            active=false; //To block next round;
            // Oczekiwanie na aktywację
            {
                std::unique_lock<std::mutex> lock(turaMtx);
                tura.wait(lock, [this]() { return active.load() || !running.load(); });
                //std::cout<<"start nowej rundy\n";
                Logger::writeMessage("start nowej rundy \n");
            }

            if (!running.load()) break;

            std::thread opponentThread(&CompetingForAttack::opponentTriesToAttack, this);
            std::thread playerThread(&CompetingForAttack::playerTriesToAttack, this);
            opponentThread.join();
            playerThread.join();
            opponentMutex.lock();
            switch (attacker) {
                case WhoAttacked::player:
                    Logger::writeMessage("player attacked\n");
                break;
                case WhoAttacked::opponent:
                    Logger::writeMessage("opponent attacked\n");
                break;
            }
            opponentMutex.unlock();
            Logger::writeMessage("end \n");
            roudEnded.store(true);
        }
    }

};
enum class Turn{User,Opponent};
#include<regex>
#include<filesystem>
#define fs std::filesystem
///


class DefaultButton : public TextDisplayer{
    //TextDisplayer buttonText;
    sf::RectangleShape outline;
    sf::Vector2f size;
public:
    DefaultButton(std::string displayedText,double sizeX, double sizeY,double posX,
        double posY):TextDisplayer("Assets/Minecraft/minecraft.ttf",displayedText,50,sf::Color::White,
            posX,posY) {
        setText(displayedText);
        setColor(sf::Color::Black);
        sf::FloatRect textBounds = TextDisplayer::getGlobalBounds();
        if (sizeY < 30) sizeY = 50;
        if (sizeX < textBounds.width + 40) sizeX = textBounds.width + 40;
        size = sf::Vector2f(sizeX, sizeY);
        outline.setSize(size);
        outline.setFillColor(sf::Color::Green);
        outline.setPosition(posX, posY);
        float centerX = posX + (sizeX - textBounds.width) / 2.f;
        float centerY = posY + (sizeY - textBounds.height) / 2.f;
        setPosition(centerX, centerY);
    }

    virtual void draw(sf::RenderWindow * master) override {
        master->draw(outline);
        TextDisplayer::draw(master);
    }
    bool isClicked(sf::RenderWindow * master) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*master);
        sf::FloatRect bounds = outline.getGlobalBounds();
        bool insideButton = bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        bool clicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        if (insideButton && clicked) {
            // std::cout << "Button " << displayName << " clicked!!!" << std::endl;
            return true;
        }
        return false;
    }
    void setPosition(double posX, double posY) {
        TextDisplayer::setPosition(posX,posY);
        this->outline.setPosition(posX,posY);
    }

};

class ListShower {
    std::vector<DefaultButton*> options;
    int page{0};
    int maxButtonsPerPage{0};
public:
    ListShower(std::string baseDirectory,sf::RenderWindow * master) {
        for (auto entry: std::filesystem::directory_iterator(std::filesystem::path(baseDirectory)) ) {
            std::string saveName=entry.path().stem().string();
            options.push_back(new DefaultButton(saveName,0.0,0.0,100.0,100.0));
        }
    }
    void listOnWindow(sf::RenderWindow * master) {
        int buttonHeight = 60;
        int spacing = 10;
        int startY = 50;
        //maxButtonsPerPage = (master->getSize().y - startY) / (buttonHeight + spacing);
        maxButtonsPerPage=4;
        for (int i = 0; i < maxButtonsPerPage; i++) {
            int index = page * maxButtonsPerPage + i;
            if (index >= options.size()) break;
            float posY = startY + i * (buttonHeight + spacing);
            options[index]->setPosition(100, posY);
        }

    }
    void resetBoundsOfPage() {
        for (int i = 0; i < maxButtonsPerPage; i++) {
            int index = page * maxButtonsPerPage + i;
            options[index]->setPosition(0.0, 0.0);
        }
    }
    void draw(sf::RenderWindow * master) {
        for (int i=0;i<maxButtonsPerPage&&page*maxButtonsPerPage+i!=options.size(); i++) {
            int index=page*maxButtonsPerPage+i;
            options[index]->draw(master);
        }
    }
    std::string handleEvents(sf::RenderWindow * master) {
        for (int i=0;i<maxButtonsPerPage&&page*maxButtonsPerPage+i!=options.size();i++) {
            if (options[page*maxButtonsPerPage+i]->isClicked(master)) {
                return options[page*maxButtonsPerPage+i]->getText();
            }
        }
        return "";
    }
    void nextPage(sf::RenderWindow * master) {
        if ((page + 1) * maxButtonsPerPage < options.size()){  page++;
            listOnWindow(master);
        }
    }

    void prevPage(sf::RenderWindow * master) {
        if (page > 0) {
            page--;
            listOnWindow(master);
        };
    }
    ~ListShower() {
        for (auto ptr: options) {
            delete ptr;
        }
    }
};

class PopOutWindow {
    DefaultButton okButton;
    TextDisplayer message;
    sf::RectangleShape background;
public:
    PopOutWindow(std::string info):okButton("OK",50,50,100,100),
    message("Assets/minecraft/Minecraft.ttf",info,30,sf::Color::Black,
        100.0f,150.0f){
        background.setPosition(100.0f,100.0f);
        background.setFillColor(sf::Color::White);
        background.setOutlineColor(sf::Color::White);
        background.setSize(sf::Vector2f(300.0f,200.0f));
        message.setText(info);
    }
    void draw(sf::RenderWindow *window) {
        window->draw(background);
        message.draw(window);
        okButton.draw(window);
    }
    void showDialog(sf::RenderWindow *window) {
        bool running=true;

        while (running) {
            sf::Event event;
            while (window->pollEvent(event)) {
                if (okButton.isClicked(window))
                    running=false;

            }
            window->clear();
            draw(window);
            window->display();

        }

    }
    void showDialog(float width, float height,std::string title) {
        auto *window = new sf::RenderWindow(sf::VideoMode(width,height),title);
        window->setFramerateLimit(60);
        background.setSize(sf::Vector2f(width,height));
        background.setFillColor(sf::Color::White);
        background.setPosition(0.0,0.0);
        okButton.setPosition(width/2-50.f,height-50.0f);
        this->message.setPosition(0.0f,10.0f);
        while (window->isOpen()) {
            sf::Event event;
            while (window->pollEvent(event)) {
                if (okButton.isClicked(window))
                    window->close();
            }
            window->clear();
            draw(window);
            window->display();
        }
        delete window;
    }

};


class Game {
    private:
        EventBinder eventsBinder;
        sf::RenderWindow gameWindow;
        GameWidgets::SceneType currentScene;
        CompetingForAttack competingForAttack;
        bool fightTriggered;
        std::thread AttackMechanics;
    void setWidgetsMaster() {
        GameWidgets::master=&gameWindow;
    }
    class UpgradeMenu {
        enum class ButtonType{attack,defense,maxHealth,goBack, weaponUpgrade, opponentUpgrade};
        std::unordered_map<ButtonType,Button*> statsButton;
        std::vector<TextDisplayer*>  statsText;
        EventBinder binder;
        void statsTextReload() {
            statsText[0]->setText("Attack: "+GameWidgets::player->getAttack());
            statsText[1]->setText("Defence : "+GameWidgets::player->getDefence());
            statsText[2]->setText("Max Health: "+std::to_string(GameWidgets::player->getMaxHealth()));
            statsText[3]->setText("Bot attack: "+GameWidgets::oponent->getAttack());
            statsText[4]->setText("Bot defence: "+GameWidgets::oponent->getDefence());
            statsText[5]->setText("Bot max Health: "+std::to_string(GameWidgets::oponent->getMaxHealth()));

        }

    public:
        bool isActive{true};
      UpgradeMenu() {

      }
        void loadAssets() {
          statsButton[ButtonType::attack]=new Button("Assets/incrementAttack.jpg",400.0f,100.0f,true);
          statsButton[ButtonType::defense]=new Button("Assets/incrementDefence.jpg", 400.0f,300.0f,true);
          statsButton[ButtonType::maxHealth]=new Button("Assets/incrementHealth.jpg",400.0f,500.0f,true);
          statsButton[ButtonType::goBack]=new Button("Assets/goBackFromUpgradeWindow.jpg" , 400.0f,700.0f,true);
          statsButton[ButtonType::weaponUpgrade]=new Button("Assets/upgradeWeapon.jpg",620.0f,500.0f,true);
          statsButton[ButtonType::opponentUpgrade] = new Button("Assets/upgradeOpponent.jpg",620.0f,700.0f,true);
          const std::string fontDirectory="Assets/Minecraft/minecraft.ttf";
          for (int i=0;i<6;i++) {
              statsText.push_back(new TextDisplayer(fontDirectory));
          }
          statsText[0]->setPosition(200.0,100.0);
          statsText[1]->setPosition(200.0,300.0);
          statsText[2]->setPosition(175.0,500.0);
          statsText[3]->setPosition(1000.0,100.0);
          statsText[4]->setPosition(1000.0,300.0);
          statsText[5]->setPosition(1000.0,500.0);
          statsTextReload();
          binder.bindAction(statsButton[ButtonType::attack],[this]() {
              if (std::stoi(GameWidgets::player->getXp())>=10) {
                  GameWidgets::player->incrementAttack();
                  GameWidgets::player->decrementXp(10);
                 statsText[0]->setText("Attack: "+GameWidgets::player->getAttack());
                  GameWidgets::xpInfo->setText("Xp: "+GameWidgets::player->getXp());
              }
              else
                  PopOutWindow("Too litle xp!\n You need at least 10\n To upgrade").showDialog(300,300," ");
          });
          binder.bindAction(statsButton[ButtonType::defense],[this]() {
              if (std::stoi(GameWidgets::player->getXp())>=10) {
                  GameWidgets::player->decrementXp(10);
                  GameWidgets::player->incrementDefense();
                   statsText[1]->setText("Defence : "+GameWidgets::player->getDefence());
                  GameWidgets::xpInfo->setText("Xp: "+GameWidgets::player->getXp());
              }
              else
                 PopOutWindow("Too litle xp!\n You need at least 10\n To upgrade").showDialog(300,300," ");
          });
          binder.bindAction(statsButton[ButtonType::maxHealth],[this]() {
            if (std::stoi(GameWidgets::player->getXp())>=10) {
                GameWidgets::player->incrementHealth();
                statsText[2]->setText(
                    "Max Health: "+std::to_string(GameWidgets::player->getMaxHealth()));
                GameWidgets::playerHealthBar->setAssociatedValue(GameWidgets::player->getMaxHealth());
                GameWidgets::playerHealthInfo->setText("HP: " +std::to_string(GameWidgets::player->getMaxHealth()));
                GameWidgets::player->decrementXp(10);
                GameWidgets::xpInfo->setText("Xp: "+GameWidgets::player->getXp());
            }
              else {
                  PopOutWindow("Too litle xp!\n You need at least 10\n To upgrade").showDialog(300,300," ");
              }
          });
          binder.bindAction(statsButton[ButtonType::goBack],[this]() {
              isActive=false;
          });
          binder.bindAction(statsButton[ButtonType::weaponUpgrade],[this]() {
              WeaponType tp=GameWidgets::playerWeapon->getType();
              WeaponUpgradeCenter center(tp);
              GameWidgets::playerWeapon=center.newWeapon(GameWidgets::playerWeapon,GameWidgets::player);
          }) ;
          binder.bindAction(statsButton[ButtonType::opponentUpgrade],[this]() {
              if (std::stoi(GameWidgets::player->getMoney())>=300&&
                  std::stoi(GameWidgets::player->getXp())>40) {
                  GameWidgets::oponent->incrementAttack();
                  GameWidgets::oponent->incrementDefense();
                  GameWidgets::oponent->incrementHealth();
                  GameWidgets::player->decrementMoney(300);
                  GameWidgets::player->decrementXp(40);
                  GameWidgets::moneyInfo->setText("Money "+GameWidgets::player->getMoney());
                  GameWidgets::xpInfo->setText("Xp "+GameWidgets::player->getXp());
                  statsTextReload();

              }
              else
                  PopOutWindow("You need at least \n 300 money & 40 xp").showDialog(300.0,300.0,"No money & xp");
          });

      }

        void drawAll(sf::RenderWindow * master) {
            std::for_each(statsButton.begin(),statsButton.end(),[this,&master](auto & B) {
                B.second->draw(master);
            });
            std::for_each(statsText.begin(),statsText.end(),[this,&master](auto & t) {
                t->draw(master);
            });
            GameWidgets::moneyInfo->draw(master);
            GameWidgets::xpInfo->draw(master);
            GameWidgets::playerWeapon->draw(master);
      }
        void handleEvents(sf::RenderWindow * master) {
            std::for_each(statsButton.begin(),statsButton.end(),[this,&master](auto & B) {
                if (B.second->isClicked(master))
                    binder.handleAction(B.second);
            });

      }
        ~UpgradeMenu() {
          delete statsButton[ButtonType::attack];
          delete statsButton[ButtonType::defense];
          delete statsButton[ButtonType::maxHealth];
          delete statsButton[ButtonType::goBack];
          std::for_each(statsText.begin(),statsText.end(),[](TextDisplayer * t) {
              delete t;
          });

      }
    };
    class DataManager {

        std::string nameInputInterface(sf::RenderWindow * window) {
            ///Creating this locally because we only use it once.
            std::regex pattern(R"([a-zA-z]+)");
            const std::string minecraftFont="Assets/Minecraft/minecraft.ttf";
            GameWidgets::newGameInput=new TextInput(100,100,300,50,window);
            TextDisplayer typeName(minecraftFont);
            typeName.setPosition(100,60);
            typeName.setText("Type in your nick");
            while (true) {
                sf::Event event;
                while (window->pollEvent(event)) {
                    GameWidgets::newGameInput->handleEvent(event);
                    if (event.type==sf::Event::EventType::KeyPressed&&event.key.code==sf::Keyboard::Enter) {
                        if (std::regex_match(GameWidgets::newGameInput->getText(),pattern))
                        return GameWidgets::newGameInput->getText();
                        else
                            typeName.setText("You can only create nick from letters");
                    }
                }
                window->clear();
                GameWidgets::newGameInput->draw();
                typeName.draw(window);
                window->display();
            }
        }
    public:
        std::string playerPath;
        std::string opponentPath;
        DataManager()=default;
        void loadData(std::string directoryPath,sf::RenderWindow * window){
            ListShower lstShower(directoryPath,window);
            lstShower.listOnWindow(window);
            std::string chosenSave;
            bool running=true;

            while (running) {
                sf::Event event;
                while (window->pollEvent(event)) {
                    std::string mes;
                    if ((mes=lstShower.handleEvents(window))!="") {
                        chosenSave=mes;
                        running=false;
                    }
                    if (event.type == sf::Event::MouseWheelScrolled) {
                        if (event.mouseWheelScroll.delta > 0) lstShower.prevPage(window);
                        if (event.mouseWheelScroll.delta < 0) lstShower.nextPage(window);
                    }
                }
                window->clear();
                lstShower.draw(window);
                window->display();
            }

            std::string path="Saves/"+chosenSave+"/playerData.txt";
            std::ifstream fileReader(path);
            playerPath=path;
            std::future<void> loadOpponent=std::async(std::launch::async,[&chosenSave,this]() {
              std::ifstream fileR("Saves/"+chosenSave+"/opponentData.txt");
                opponentPath="Saves/"+chosenSave+"/opponentData.txt";
              //GameWidgets::oponent=new OldPlayer(fileR);
              GameWidgets::oponent=new Fighter(fileR);
          });
           // GameWidgets::player=new OldPlayer(fileReader);
            GameWidgets::player=new Player(fileReader);
            //GameWidgets::oponent=new Player("CPU2000","Assets/opponentSkin.jpg", 1,1,0,0,100,100,1);
            loadOpponent.get();
            GameWidgets::playerNameText->setText(chosenSave);
            GameWidgets::playerHealthBar=new HealthBar({120.0,20.0},
        {200.0,300.0},GameWidgets::player->getMaxHealth());
            GameWidgets::opponentHealthBar=new HealthBar({120.0,20.0},
                {600.0,300.0},GameWidgets::oponent->getMaxHealth());
            GameWidgets::playerHealthInfo=new TextDisplayer("Assets/Minecraft/minecraft.ttf");
              GameWidgets::playerHealthInfo->setText("HP: "+GameWidgets::player->getHealth());
            GameWidgets::playerHealthInfo->setPosition(200.0,250.0);
            GameWidgets::opponentHealthInfo=new TextDisplayer("Assets/Minecraft/minecraft.ttf");
            GameWidgets::opponentHealthInfo->setText("HP: "+GameWidgets::oponent->getHealth());
            GameWidgets::opponentHealthInfo->setPosition(600.0,250.0);
            GameWidgets::moneyInfo->setText("Money: "+GameWidgets::player->getMoney());
            GameWidgets::xpInfo->setText("XP Points: "+GameWidgets::player->getXp());
            GameWidgets::moneyInfo->setPosition(700.0,50.0);
            GameWidgets::xpInfo->setPosition(100.0,50.0);
            std::string weaponPath=std::filesystem::path(playerPath).parent_path().string();
            std::string oponentWeaponPath=weaponPath;
            weaponPath+="/weaponData.json";
            oponentWeaponPath+="/opponentWeaponData.json";
            std::cout<<weaponPath<<"\n";
            nlohmann::json wJson;
            wJson= wJson.parse(std::ifstream(weaponPath));
            GameWidgets::playerWeapon->loadFromJson(wJson);
            wJson=wJson.parse(std::ifstream(weaponPath));
            GameWidgets::oponentWeapon->loadFromJson(wJson);

        }
        void createData(sf::RenderWindow * window) {
            bool accepted=false;
            std::string name;
            while (!accepted) {
               name=nameInputInterface(window);
                playerPath="Saves/";
                playerPath+=name;
                if (!fs::exists(playerPath)) {
                    fs::create_directory(fs::path(playerPath));
                    playerPath+="/playerData.txt";
                    std::ofstream(currentPath);
                    opponentPath="Saves/";
                    opponentPath+=name;
                    opponentPath+="/opponentData.txt";
                    std::ofstream(opponentPath);
                    accepted=true;
                }
                else {
                    playerPath.clear();
                    PopOutWindow("This save\n already exists!").showDialog(200.0f,
                        200.0f,
                        "Try again");
                }
            }
           // GameWidgets::player=new OldPlayer(name,"Assets/playerSkin.jpg",5,3,0,0,100,100,0);
          //  GameWidgets::oponent=new OldPlayer("CPU2000","Assets/opponentSkin.jpg",1,1,0,0,100,100,1);
            GameWidgets::player=new Player(name,"Assets/playerSkin.jpg",5,3,100,100,0,1,0,0,0);
            GameWidgets::oponent=new Fighter("CPU2000","Assets/opponentSkin.jpg",1,1,100,100,1,1,0, 0 );
            GameWidgets::playerNameText->setText(name);
            GameWidgets::playerHealthBar=new HealthBar({120.0,20.0},
        {200.0,300.0},GameWidgets::player->getMaxHealth());
            GameWidgets::opponentHealthBar=new HealthBar({120.0,20.0},
                {600.0,300.0},GameWidgets::oponent->getMaxHealth());
            GameWidgets::playerHealthInfo=new TextDisplayer("Assets/Minecraft/minecraft.ttf");
            GameWidgets::playerHealthInfo->setText("HP: "+GameWidgets::player->getHealth());
            GameWidgets::playerHealthInfo->setPosition(200.0,250.0);
            GameWidgets::opponentHealthInfo=new TextDisplayer("Assets/Minecraft/minecraft.ttf");
            GameWidgets::opponentHealthInfo->setText("HP: "+GameWidgets::oponent->getHealth());
            GameWidgets::opponentHealthInfo->setPosition(600.0,250.0);
            GameWidgets::moneyInfo->setText("Money: "+GameWidgets::player->getMoney());
            GameWidgets::xpInfo->setText("XP Points: "+GameWidgets::player->getXp());
            GameWidgets::moneyInfo->setPosition(700.0,50.0);
            GameWidgets::xpInfo->setPosition(100.0,50.0);


             }
        void saveData() {
            std::future<void> saveOpponentData=std::async(std::launch::async,[this]() {
                std::string savePath=opponentPath;
                GameWidgets::oponent->saveData(savePath);
                std::string weaponPathA=std::filesystem::path(playerPath).parent_path().string();
                weaponPathA+="/opponentWeaponData.json";
           std::ofstream fout(weaponPathA);
           fout<<GameWidgets::oponentWeapon->getJson().dump(1);

            });
            std::string pth=playerPath;
            GameWidgets::player->saveData(pth);
            std::filesystem::path pthN(pth);
            ///Creating Weapon Path
            std::string weaponPath=pthN.parent_path().string();
            weaponPath+="/weaponData.json";
            std::cout<<weaponPath;
            std::ofstream fout(weaponPath);
            fout<<GameWidgets::playerWeapon->getJson().dump(1);
            saveOpponentData.get();

        }

    };
    DataManager dataManager;
    UpgradeMenu upgradeMenu;
    void handleUpgradeWindow() {
        ///TODO , do the buttons in SingleTone not locally;
        ///

        while (upgradeMenu.isActive) {
            sf::Event event;
            while (gameWindow.pollEvent(event)) {
                upgradeMenu.handleEvents(&gameWindow);
            }
            gameWindow.clear();
            upgradeMenu.drawAll(&gameWindow);
            gameWindow.display();
        }
        upgradeMenu.isActive=true;
    }
    void bindEvents() {
        eventsBinder.bindAction(GameWidgets::attackButton,[this]() {
           competingForAttack.active=true;
           fightTriggered=true;
           competingForAttack.playerCV.notify_all();
           competingForAttack.tura.notify_all();
       });
        eventsBinder.bindAction(GameWidgets::loadGame,[this]() {
            dataManager.loadData("Saves",&gameWindow);
                ///TODO make other logic for now this
               // dataManager.createPlayerData(&gameWindow);
                GameWidgets::newGame->deactivate();
                currentScene=GameWidgets::SceneType::duelScene;
                GameWidgets::attackButton->activate();
                PopOutWindow popOut("To start round\n click attack\n ");
                popOut.showDialog(&gameWindow);
                PopOutWindow popOut2("To attack press A\n");
                popOut2.showDialog(&gameWindow);
                PopOutWindow popOut3("To upgrade \n  press S");
                popOut3.showDialog(&gameWindow);
                upgradeMenu.loadAssets();
        });
        eventsBinder.bindAction(GameWidgets::newGame,[this]() {
            dataManager.createData(&gameWindow);
                GameWidgets::newGame->deactivate();
                currentScene=GameWidgets::SceneType::duelScene;
                upgradeMenu.loadAssets();
                GameWidgets::attackButton->activate();
        });
        eventsBinder.bindAction(GameWidgets::playerWeapon,[this]() {

        });
        eventsBinder.bindAction(GameWidgets::saveAndExit,[this]() {
            gameWindow.close();

        }
            );
    }

    void listenToEvents() {

        static std::vector<Button *> widgets{GameWidgets::attackButton,
            GameWidgets::loadGame,
            GameWidgets::newGame,
            GameWidgets::saveAndExit
        };

        sf::Event event;
        static int releaseConter=0;

        while (gameWindow.pollEvent(event)) {
            if (event.type==sf::Event::KeyPressed&&event.key.code==sf::Keyboard::S) {
                handleUpgradeWindow();
            }
            if (event.type==sf::Event::Closed) {
                competingForAttack.running=false;
                competingForAttack.playerCV.notify_all();
                competingForAttack.tura.notify_all();
                dataManager.saveData();
                gameWindow.close();
            }

            for (auto & btn: widgets) {
                if (btn->isClicked(&gameWindow)&&btn->isActive())
                eventsBinder.handleAction(btn);
            }
             if (GameWidgets::playerWeapon->isClicked(&gameWindow)) {
                if (releaseConter<1){
                    std::thread([&]() {

                        PopOutWindow(GameWidgets::playerWeapon->getInfo()).showDialog(
                            350.0f,350.0f,"why");
                        releaseConter--;
                    }).detach();
             releaseConter++;
                }
            }
        }

    }
    void resetAllInfo() {
        fightTriggered=false;
        competingForAttack.active=false;
        competingForAttack.decisionLocked=false;
        GameWidgets::player->resetStats();
        GameWidgets::oponent->resetStats();
        GameWidgets::playerHealthBar->reset();
        GameWidgets::opponentHealthBar->reset();
        GameWidgets::playerHealthInfo->setText("HP: "+GameWidgets::player->getHealth());
        GameWidgets::opponentHealthInfo->setText("HP: "+ GameWidgets::oponent->getHealth());

    }



    void startFightRound() {
        if (!fightTriggered)
            return;
        while (fightTriggered) {
            sf::Event event;
            while (gameWindow.pollEvent(event)) {
                if (event.type==sf::Event::KeyReleased&&event.key.code==sf::Keyboard::A) {
                    competingForAttack.playerPressed=true;
                    competingForAttack.playerCV.notify_all();
                }
                if (event.type==sf::Event::Closed) {
                    competingForAttack.running=false;
                    competingForAttack.active=false;
                    fightTriggered=false;
                    competingForAttack.playerCV.notify_all();
                    competingForAttack.tura.notify_all();
                    dataManager.saveData();
                    gameWindow.close();
                    continue;
                }
            }
            if (competingForAttack.roudEnded.load()){
                competingForAttack.roudEnded=false;
                 if (competingForAttack.attacker.load()==CompetingForAttack::WhoAttacked::player) {
                     bool fightResult=
                         GameWidgets::player->fight(GameWidgets::oponent,
                             GameWidgets::playerDice->roll(&gameWindow));
                     GameWidgets::playerWeapon->attackAnimation(&gameWindow);
                     GameWidgets::opponentHealthInfo->setText("HP: "+ GameWidgets::oponent->getHealth());
                     if (fightResult) {
                         GameWidgets::resultDisplayer->drawResult(true,&gameWindow);
                         resetAllInfo();
                          break;
                    }
                }
                else  {
                    competingForAttack.playerPressed=false;
                    bool fightResult=GameWidgets::oponent->fight(GameWidgets::player,GameWidgets::opponentDice->roll(&gameWindow));
                    GameWidgets::playerHealthInfo->setText("HP: "+GameWidgets::player->getHealth());
                    if (fightResult) {
                        GameWidgets::resultDisplayer->drawResult(false,&gameWindow);
                        resetAllInfo();
                        break;
                    }
                }
                competingForAttack.active=true;
                sf::sleep(sf::milliseconds(10));
                competingForAttack.tura.notify_all();
            }
        }

    }

    public:
    Game(): gameWindow(/*sf::VideoMode(1000.0,1000.0)*/ sf::VideoMode::getDesktopMode(),"DuelBattle"),
       currentScene(GameWidgets::SceneType::mainMenu),fightTriggered(false),AttackMechanics(&CompetingForAttack::main,&competingForAttack)
            {
    }

    void startMenu() {
                setWidgetsMaster();
                mainLoop();
        }
    void mainLoop() {
                bindEvents();
                while (gameWindow.isOpen()) {
                    listenToEvents();
                    gameWindow.clear();
                    GameWidgets::drawScene(currentScene);
                    startFightRound();
                    gameWindow.display();
                }
                competingForAttack.running=false;
                competingForAttack.tura.notify_all();
                AttackMechanics.join();
            }
    friend void debbugingMode(sf::RenderWindow * window);
    friend void debugUpgradeWin(sf::RenderWindow * window);
};

inline void debugUpgradeWin(sf::RenderWindow *window) {
    Game::UpgradeMenu upgradeMenu;
    upgradeMenu.loadAssets();
    ///No worries it self actualizes
    while (upgradeMenu.isActive) {
        sf::Event event;
        while (window->pollEvent(event)) {
            upgradeMenu.handleEvents(window);
        }
        window->clear();
        upgradeMenu.drawAll(window);
        window->display();

    }

}

void debbugingMode(sf::RenderWindow * window) {
    const std::string minecraftFont= "Assets/minecraft/Minecraft.ttf";
    const std::string name="testowy";
   // GameWidgets::player=new OldPlayer(name,"Assets/playerSkin.jpg",5,3,0,0,100,100,0);
    //GameWidgets::oponent=new OldPlayer("CPU2000","Assets/opponentSkin.jpg",1,1,0,0,100,100,1);
    GameWidgets::player=new Player(name,"Assets/playerSkin.jpg",5,3,0,0,100,100,0);
    GameWidgets::oponent=new Fighter("CPU2000","Assets/opponentSkin.jpg",1,1,0,0,100,100,1);
    GameWidgets::playerNameText->setText(name);
    GameWidgets::playerHealthBar=new HealthBar({120.0,20.0},
{200.0,300.0},GameWidgets::player->getMaxHealth());
    GameWidgets::opponentHealthBar=new HealthBar({120.0,20.0},
        {600.0,300.0},GameWidgets::oponent->getMaxHealth());
    GameWidgets::playerHealthInfo=new TextDisplayer("Assets/Minecraft/minecraft.ttf");
    GameWidgets::playerHealthInfo->setText("HP: "+GameWidgets::player->getHealth());
    GameWidgets::playerHealthInfo->setPosition(200.0,250.0);
    GameWidgets::opponentHealthInfo=new TextDisplayer("Assets/Minecraft/minecraft.ttf");
    GameWidgets::opponentHealthInfo->setText("HP: "+GameWidgets::oponent->getHealth());
    GameWidgets::opponentHealthInfo->setPosition(600.0,250.0);
    GameWidgets::moneyInfo->setText("Money: "+GameWidgets::player->getMoney());
    GameWidgets::xpInfo->setText("XP Points: "+GameWidgets::player->getXp());
    GameWidgets::moneyInfo->setPosition(700.0,50.0);
    GameWidgets::xpInfo->setPosition(100.0,50.0);
}





#endif //GAME_H

#include <assert.h>
#include<SFML/Window/Window.hpp>
#include<SFML/Window/VideoMode.hpp>
#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<vector>
#include<thread>
#include<future>
#include<iostream>
#include<fstream>
#include<SFML/System/FileInputStream.hpp>
#include<SFML/Audio.hpp>
using std::string;
#include<queue>
#include<map>
#include<nlohmann/json.hpp>
#include"basicClasses.h"
#include"WindowMonitor.h"
#include"GameMechanicWidgets.h"
//#include"CrucialData.h"
#include"Game.h"
#include"sound.h"
#include"easierDesign.h"



bool OldPlayer::fight(OldPlayer * opponent, int attackFactor) {
    int calculatedAttack=(attackFactor*attackPoints+
        GameWidgets::playerWeapon->getAttackFactor(this))-
            opponent->defensePoints;
    if (calculatedAttack>0) {
        opponent->health-=(opponent->health-calculatedAttack>0?calculatedAttack:opponent->health); ///Simple decrement of life
        if (opponent->name=="CPU2000")
            GameWidgets::opponentHealthBar->decrement(calculatedAttack);
        else
            GameWidgets::playerHealthBar->decrement(calculatedAttack);
    }
    if (opponent->health<=0) {
        if (this->name!="CPU2000") {
            this->xpPoints+=10.0;
            this->money+=100;
            GameWidgets::moneyInfo->setText("Money: "+std::to_string(money));
            GameWidgets::xpInfo->setText("XP Points: "+std::to_string(xpPoints));
        }
        return true;
    }
    return false;

}
#include<array>
int Dice::roll(sf::RenderWindow * master) {
    {
        for (int i=0;i<10;i++) {
            master->clear();
            currentSide=rand()%6;
            setTexture(*GameWidgets::diceTextures[currentSide]);
            GameWidgets::SceneType sctype=GameWidgets::SceneType::duelScene;
            GameWidgets::drawScene(sctype);
            master->display();
            sf::sleep(sf::milliseconds(30));
        }
        //rolled=false;
        return currentSide+1;
    }

}

void AnimationsLibrary::display(sf::RenderWindow *window, sf::Sprite *where)
{
    for (auto & frame : frames) {
        window->clear();
        GameWidgets::SceneType sctype=GameWidgets::SceneType::duelScene;
        GameWidgets::drawScene(sctype);
        where->setTexture(*frame);
        window->draw(*where);
        window->display();
    }

}

void ShotGun::attackAnimation(sf::RenderWindow *window, int attacker) {


        static GraphicalObject pStickmin("Assets/animationSprites/pS.png",30,800);

        ///Deciding which figure to move

        std::string rawAssetPath="Assets/animationSprites/";
        rawAssetPath+= OpponentUpgradeCenter::charactersNames[
                GameWidgets::oponent->getLevel()];
        rawAssetPath+=".png";
        GraphicalObject oStickmin(rawAssetPath,500,800);
        GraphicalObject pWeapon("Assets/animationSprites/shotgun.png",200,840);
    if (attacker==0) {
        // oStickmin.setMask(sf::Color::White);
        // pStickmin.setMask(sf::Color::Black);
        //  pWeapon.setMask(sf::Color::Black);

        sf::CircleShape shape(3);
        shape.setFillColor(sf::Color::White);
        shape.setPosition(200.0f+pWeapon.getSize().x+3,860.0f);
        sf::Vector2f destination={500.0f+oStickmin.getSize().x-120,800.f};
        window->setFramerateLimit(40);
        while (shape.getPosition().x<destination.x) {

            shape.move(4.0f,0.0f);
            window->clear();
            GameWidgets::SceneType sctype=GameWidgets::SceneType::duelScene;
            GameWidgets::drawScene(sctype);
            window->draw(shape);
            oStickmin.draw(window);
            pWeapon.draw(window);
            pStickmin.draw(window);
            window->display();
        }
    }
    else {
        pWeapon.flip();
        pWeapon.setPosition(450,840);
        sf::CircleShape shape(3);
        shape.setFillColor(sf::Color::White);
        shape.setPosition(500.0f-pWeapon.getSize().x-3,860.0f);
        sf::Vector2f destination={30.0f+oStickmin.getSize().x-120,800.f};
        window->setFramerateLimit(40);
        while (shape.getPosition().x>destination.x) {

            shape.move(-4.0f,0.0f);
            window->clear();
            GameWidgets::SceneType sctype=GameWidgets::SceneType::duelScene;
            GameWidgets::drawScene(sctype);
            window->draw(shape);
            oStickmin.draw(window);
            pWeapon.draw(window);
            pStickmin.draw(window);
            window->display();
        }

    }

    window->setFramerateLimit(20);
}
//#define test



#include<SFML/Audio.hpp>

void Saber::attackAnimation(sf::RenderWindow *window, int attacker)  {

        std::string rawAssetPath="Assets/animationSprites/";
        rawAssetPath+= OpponentUpgradeCenter::charactersNames[
            GameWidgets::oponent->getLevel()];
        rawAssetPath+=".png";
        std::ifstream input;
        if (!attacker)
        input.open("Assets/animationSprites/swordAnimationPlayer.json");
        else
            input.open("Assets/animationSprites/swordAnimationOpponent.json");
        nlohmann::json json;
        input>>json;
        nlohmann::json initial=*json.begin();

        try {
            GraphicalObject oStickmin(rawAssetPath,initial["opponent"]["x"],
                    initial["opponent"]["y"]);
            GraphicalObject pWeapon("Assets/animationSprites/sword.png",
                initial["weapon"]["x"],initial["weapon"]["y"]);
            pWeapon.setRotation(initial["weapon"]["rotation"]);
            GraphicalObject pStickmin("Assets/animationSprites/pS.png",
                initial["player"]["x"],
                initial["player"]["y"]);
            if (attacker)
                pWeapon.flip();

            std::future<void> soundAsync=std::async(std::launch::async,
                playWav,"Assets/Music/swordSound.mp3",1500);

            for (auto & j:json) {

                oStickmin.setPosition(j["opponent"]["x"],j["opponent"]["y"]);
                pStickmin.setPosition(j["player"]["x"],j["player"]["y"]);
                pWeapon.setPosition(j["weapon"]["x"],j["weapon"]["y"]);
                pWeapon.setRotation(j["weapon"]["rotation"]);
                window->clear();
                GameWidgets::SceneType sctype=GameWidgets::SceneType::duelScene;
                GameWidgets::drawScene(sctype);
                oStickmin.draw(window);
                pStickmin.draw(window);
                pWeapon.draw(window);

                window->display();
            }
            soundAsync.get();
            if (attacker)
                pWeapon.flip();

        }
        catch (std::exception & e) {
            std::cout << e.what() << std::endl;
        }



}

OldPlayer::OldPlayer(std::ifstream &filePath, std::string skinAssetPath, float posX, float posY) :id(0),
visible(true),
GraphicalObject(skinAssetPath,posX,posY) {

    filePath>>name>>attackPoints>>defensePoints>>xpPoints>>maxHealth>>money;
    health=maxHealth;
    int weaponType;
    filePath>>weaponType;
    switch(weaponType) {
        case 1:
            GameWidgets::playerWeapon=new ShotGun();
        break;
        case 2:
            GameWidgets::playerWeapon=new Saber();
        break;
        case 3:
            GameWidgets::playerWeapon=new Wand();
        break;
    }
}

void OldPlayer::saveData(std::string  filePath)  {
    const char space='\t';
    std::ofstream file(filePath);
    // std::cout<<"Nazwa Pliku to"<<filePath<<std::endl;
    file<<name<<space<<attackPoints<<space<<defensePoints<<space<<xpPoints<<space<<maxHealth<<
        space<<money;
    switch (GameWidgets::playerWeapon->getType()) {
        case WeaponType::shotgun:
            file<<space<<1;
        break;
        case WeaponType::saber:
            file<<space<<2;
        break;
        case WeaponType::wand:
            file<<space<<3;
        break;
    }


}

Weapon *WeaponUpgradeCenter::newWeapon(Weapon *weapon, OldPlayer *player)

{
    auto reduce=[this,&player](){player->decrementMoney(prices[current]);
        GameWidgets::moneyInfo->setText(player->getMoney());};
    if (std::stoi(player->getMoney())>=prices[current]) {
        switch (current) {
            case WeaponType::shotgun:
                if (weapon->selfUpgrade()) {
                    reduce();
                    return weapon;
                }
                reduce();
                return new Saber();
            case WeaponType::saber:
                if (weapon->selfUpgrade()) {
                    reduce();
                    return weapon;
                }
                reduce();
                return new Wand();
            case WeaponType::wand:
                if (weapon->selfUpgrade()) {
                    reduce();
                    return weapon;
                }
            PopOutWindow("Reached Max Level!").showDialog(200.0f,200.0f,"MAXED");
                return weapon;
        }
        return weapon;

    }
    else {
        PopOutWindow info("You need more money!\n You need\n  at least"+std::to_string(prices[current]));
        info.showDialog(300.0f,300.0f,"No money");

        //info.showDialog(GameWidgets::master);
        return weapon;
    }

}


Weapon *WeaponUpgradeCenter::newWeapon(Weapon *weapon, Player *player) {

    auto reduce=[this,&player](){player->decrementMoney(prices[current]);
        GameWidgets::moneyInfo->setText(player->getMoney());};
    if (std::stoi(player->getMoney())>=prices[current]) {
        switch (current) {
            case WeaponType::shotgun:
                if (weapon->selfUpgrade()) {
                    reduce();
                    return weapon;
                }
            reduce();
            return new Saber();
            case WeaponType::saber:
                if (weapon->selfUpgrade()) {
                    reduce();
                    return weapon;
                }
            reduce();
            return new Wand();
            case WeaponType::wand:
                if (weapon->selfUpgrade()) {
                    reduce();
                    return weapon;
                }
            PopOutWindow("Reached Max Level!").showDialog(200.0f,200.0f,"MAXED");
            return weapon;
        }
        return weapon;

    }
    else {
        PopOutWindow info("You need more money!\n You need\n  at least"+std::to_string(prices[current]));
        info.showDialog(300.0f,300.0f,"No money");

        //info.showDialog(GameWidgets::master);
        return weapon;
    }


}


void Wand::attackAnimation(sf::RenderWindow *window, int attacker) {


   static  GraphicalObject pS("Assets/animationSprites/pS.png",0.0f,0.0f);
    std::string rawAssetPath="Assets/animationSprites/";
    rawAssetPath+= OpponentUpgradeCenter::charactersNames[
        GameWidgets::oponent->getLevel()];
    rawAssetPath+=".png";
    GraphicalObject oS(rawAssetPath,0.0f,0.0f);
    static GraphicalObject pW("Assets/animationSprites/wand.png",0.0f,0.0f);
    GraphicalObject * spell;

    std::ifstream input;
    nlohmann::json json;
    std::function<void()> musicEffect;
        switch (currentSpell) {
            case Spell::fireball:
                spell=new GraphicalObject("Assets/animationSprites/fireball.png",0.0f,0.0f);
                if (!attacker)
                    input.open("Assets/animationSprites/wandAnimationPlayerFire.json");
                else
                    input.open("Assets/animationSprites/wandAnimationOpponentFire.json");
            input>>json;
            musicEffect = [&]() {
               playWav("Assets/Music/fire.mp3",2200);
            };
            break;
            case Spell::shield:

                spell=new GraphicalObject("Assets/animationSprites/shield.png",0.0f,0.0f);
                if (!attacker)
                    input.open("Assets/animationSprites/wandAnimationPlayerShield.json");
                else
                    input.open("Assets/animationSprites/wandAnimationOpponentShield.json");
            input>>json;

            musicEffect= [&]() {
               playWav("Assets/Music/shield.mp3");
            };
            break;
            case Spell::snowball:
                spell=new GraphicalObject("Assets/animationSprites/snowball.png",0.0f,0.0f);
                if (!attacker)
                    input.open("Assets/animationSprites/wandAnimationPlayerFire.json");
                else
                    input.open("Assets/animationSprites/wandAnimationOpponentFire.json");
            input>>json;

            musicEffect =[&]() {
               playWav("Assets/Music/snowball.mp3");
            };
            break;
        }
        try {
            if (attacker) {
                pW.flip();
                spell->flip();
            }

            int frameCounter=0;
            std::future<void> musicFuture;
            for (auto & j:json) {

                oS.setPosition(j["opponent"]["x"],j["opponent"]["y"]);
                pS.setPosition(j["player"]["x"],j["player"]["y"]);
                pW.setPosition(j["weapon"]["x"],j["weapon"]["y"]);
                pW.setRotation(j["weapon"]["rotation"]);
                spell->setPosition(j["spell"]["x"],j["spell"]["y"]);
                window->clear();
                GameWidgets::SceneType sctype=GameWidgets::SceneType::duelScene;
                GameWidgets::drawScene(sctype);
                oS.draw(window);
                pS.draw(window);
                pW.draw(window);
                spell->draw(window);
                window->display();
                frameCounter++;
                if (frameCounter ==5) {

               musicFuture= std::async(std::launch::async,  musicEffect);
                }
            }
            musicFuture.get();
            if (attacker) {
                pW.flip();
                spell->flip();
            }
            delete spell;
        }
        catch (std::exception &e) {
            std::cerr<<"Failed to load animations";
        }

}


Player::Player(std::ifstream &filePath, std::string skinAssetPath, float posX, float posY):
Fighter("",skinAssetPath,posX,posY)
{
    filePath>>name>>attackPoints>>defensePoints>>xp>>maxHealth>>money;
    health=maxHealth;
    int weaponType;
    filePath>>weaponType;
    switch(weaponType) {
        case 1:
            GameWidgets::playerWeapon=new ShotGun();
        break;
        case 2:
            GameWidgets::playerWeapon=new Saber();
        break;
        case 3:
            GameWidgets::playerWeapon=new Wand();
        break;
    }
}

void Player::saveData(std::string  filePath)  {
    const char space='\t';
    std::ofstream file(filePath);
    file<<name<<space<<attackPoints<<space<<defensePoints<<space<<xp<<space<<maxHealth<<
        space<<money;

    file<<space<<GameWidgets::playerWeapon->getNumericalType();
    /*
    switch (GameWidgets::playerWeapon->getType()) {
        case WeaponType::shotgun:
            file<<space<<1;
        break;
        case WeaponType::saber:
            file<<space<<2;
        break;
        case WeaponType::wand:
            file<<space<<3;
        break;
    }
    */
}

bool Player::fight(Fighter *opponent, int attackFactor) {
    int calculatedAttack=(attackFactor*attackPoints+
       GameWidgets::playerWeapon->getAttackFactor(this))-
           opponent->getDefenceI();
    if (calculatedAttack>0) {
        opponent->reduceHealth(opponent->getHealthI()-calculatedAttack>0?calculatedAttack:opponent->getHealthI()); ///Simple decrement of life
            GameWidgets::opponentHealthBar->decrement(calculatedAttack);
    }
    if (opponent->getHealthI()<=0) {

            this->xp+=10.0;
            this->money+=100;
            GameWidgets::moneyInfo->setText("Money: "+std::to_string(money));
            GameWidgets::xpInfo->setText("XP Points: "+std::to_string(xp));

        return true;
    }
    return false;

}


bool Fighter::fight(Fighter *opponent, int attackFactor) {
    int calculatedAttack=(attackFactor*attackPoints+
       GameWidgets::playerWeapon->getAttackFactor(this)-
           opponent->getDefenceI());
    if (calculatedAttack>0) {
        opponent->reduceHealth(opponent->getHealthI()-calculatedAttack>0?calculatedAttack:opponent->getHealthI()); ///Simple decrement of life
        GameWidgets::playerHealthBar->decrement(calculatedAttack);

    }

    return opponent->getHealthI()<=0;
}

Fighter::Fighter(std::ifstream &filePath, std::string skinAssetPath, float posX, float posY): GraphicalObject(skinAssetPath,posX,posY) {
    filePath>>name>>attackPoints>>defensePoints>>maxHealth>>maxAttack>>maxDefense>>level;
    health=maxHealth;
    int type;
    filePath>>type;
    switch(type) {
        case 1:
            GameWidgets::oponentWeapon=new ShotGun();
       // GameWidgets::oponentWeapon->setPosition(1200.0,800.0);
        break;
        case 2:
            GameWidgets::oponentWeapon=new Saber();
       // GameWidgets::oponentWeapon->setPosition(1200.0,800.0);

        break;
        case 3:
            GameWidgets::oponentWeapon=new Wand();
      //  GameWidgets::oponentWeapon->setPosition(1200.0,800.0);

        break;
    }





}
void Fighter::saveData(std::string filePath) {
    const char space='\t';
    std::ofstream file(filePath);
    file<<name<<space<<attackPoints<<space<<defensePoints<<space<<space<<maxHealth<<space
    <<maxAttack<<space<<maxDefense<<space<<level;
    file<<space<<GameWidgets::oponentWeapon->getNumericalType();
    /*
    switch (GameWidgets::oponentWeapon->getType()) {
        case WeaponType::shotgun:
            file<<space<<1;
        break;
        case WeaponType::saber:
            file<<space<<2;
        break;
        case WeaponType::wand:
            file<<space<<3;
        break;
    }
*/
}


void OpponentUpgradeCenter::newOpponent(Fighter *current, Player *player) {
    int currentLevel=current->getLevel();

    if (std::stoi(player->getMoney())>=moneyAndXpThresholds[currentLevel][0]&&
                std::stoi(player->getXp())>=moneyAndXpThresholds[currentLevel][1]&&currentLevel<5) {
        player->decrementMoney(moneyAndXpThresholds[currentLevel][0]);
        player->decrementXp(moneyAndXpThresholds[currentLevel][1]);

        if (!GameWidgets::oponentWeapon->selfUpgrade()) {
            sf::Vector2f posA=GameWidgets::oponentWeapon->getPosition();
            switch (GameWidgets::oponentWeapon->getType()) {
                case WeaponType::shotgun:
                    GameWidgets::oponentWeapon=new Saber();
                    GameWidgets::oponentWeapon->setPosition(posA.x,posA.y);
                break;
                case WeaponType::saber:
                    GameWidgets::oponentWeapon=new Wand();
                    GameWidgets::oponentWeapon->setPosition(posA.x,posA.y);
                break;
                default:
                    break;
            }
        }
        if (!current->selfUpgrade()) {
                current->setMaxAttack(current->getAttackI()+5);
                current->setMaxDefense(current->getDefenceI()+5);
                if (currentLevel<4) {
                    current->incrementLevel();
                    sf::Texture newTexture;
                    newTexture.loadFromFile(charactersTexturePaths[++currentLevel]);
                    current->setTexture(newTexture);
                    GameWidgets::opponentNameText->setText(charactersNames[++currentLevel]);
                }
            else {
                PopOutWindow("You reached last character\n You're now in the endless zone \n More characters coming soon")
                .showDialog(500.f,400.0f,"MAX");
            }

        }
                }
    else {
        PopOutWindow("Need more \n xp and money").showDialog(300.0f,300.0f,"GIMME MONEY");
    }

}


#define gameW
//#define test1



int main(int avg, char ** args) {
#ifdef  test1
    sf::Texture textureA;
    textureA.loadFromFile("Assets/animationSprites/Shrek.png");
    sf::Texture textureB;
    textureB.loadFromFile("Assets/animationSprites/pS.png");
    sf::Texture textureC;
    textureC.loadFromFile("Assets/animationSprites/wand.png");
    sf::Texture textureD;
    textureD.loadFromFile("Assets/animationSprites/shield.png");
    DraggableObject stickmanPlayer(textureB,sf::Vector2f(100.0f,100.0f));
    DraggableObject stickmanO(textureA,sf::Vector2f(300.0f,100.0f));
    DraggableObject weapon(textureC,sf::Vector2f(500.0f,100.0f));
    DraggableObject spell(textureD,sf::Vector2f(700.0f,100.0f));
    //weapon.flip();
    nlohmann::json data;
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(),"GIMME MONEY");
    int frame=0;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyReleased&&event.key.code==sf::Keyboard::Space) {

                stickmanPlayer.savePosition(data[frame]["player"]);
                stickmanO.savePosition(data[frame]["opponent"]);
                weapon.savePosition(data[frame]["weapon"]);
                data[frame]["weapon"]["rotation"]=weapon.sprite.getRotation();
                spell.savePosition(data[frame]["spell"]);
                frame++;
            }
            if (event.type==sf::Event::KeyReleased&&event.key.code==sf::Keyboard::Right) {

                weapon.sprite.rotate(1);
            }
            if (event.type==sf::Event::KeyReleased&&event.key.code==sf::Keyboard::Left) {
                weapon.sprite.rotate(-1);
            }
            stickmanPlayer.handleEvent(event, window);
            stickmanO.handleEvent(event, window);
            weapon.handleEvent(event, window);
            spell.handleEvent(event, window);
        }

        stickmanPlayer.update(window);
        stickmanO.update(window);
        weapon.update(window);
        spell.update(window);
        window.clear(sf::Color::White);
        stickmanPlayer.draw(window);
        stickmanO.draw(window);
        weapon.draw(window);
        spell.draw(window);
        window.display();
    }
std::ofstream of("Assets/animationSprites/wandAnimationPlayerShield.json");
    of<<data.dump(1);

#endif

#ifdef gameW

    GameWidgets::loadUIFromJson("Assets/UI/main.json");
    Game game;
    game.startMenu();
    GameWidgets::release();

    return 0;
#endif
#ifdef test

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Demo");
    UIFactory factory({1920, 1080}, window.getSize());
    std::ifstream f("ui_config.json");
    nlohmann::json jsonData = nlohmann::json::parse(f);
    factory.loadFromJson(jsonData);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

        }
        window.clear();
        factory.at<TextDisplayer*>("moneyInfo")->draw(&window);
        window.display();
    }




#endif

    return 0;


}
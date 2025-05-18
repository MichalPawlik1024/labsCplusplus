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
#include"CrucialData.h"
#include"Game.h"


//#define gameW
///Crucial Definitions that need to be here because they use the CrucialData.h
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
    sf::CircleShape shape(3);
    shape.setFillColor(sf::Color::White);
    shape.setPosition(300.0f,600.0f);
    sf::Vector2f destination={500.0f,600.f};
    window->setFramerateLimit(20);
    while (shape.getPosition().x<destination.x) {
        shape.move(4.0f,0.0f);
        window->clear();
         GameWidgets::SceneType sctype=GameWidgets::SceneType::duelScene;
         GameWidgets::drawScene(sctype);
            window->draw(shape);
            window->display();

    }
    /*
    sf::Sprite sprite; sprite.setPosition(1000.0f,200.0f);
    AnimationsLibrary animations;
    animations.loadFromDir("Assets/shotgunAnimation/");
    animations.display(window,&sprite);*/
}
//#define test



#include<SFML/Audio.hpp>

void Saber::attackAnimation(sf::RenderWindow *window, int attacker)  {
        sf::Sprite sprite; sprite.setPosition(1000.0f,200.0f);
        static  AnimationsLibrary animations;
        animations.loadFromDir("Assets/player/");

        animations.display(window,&sprite);
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
    AnimationsLibrary animations;
    if (!attacker) {
        switch (currentSpell) {
            case Spell::fireball:
                animations.loadFromDir("Assets/wand/fireball");
            break;
            case Spell::shield:
                animations.loadFromDir("Assets/wand/shield");
            break;
            case Spell::snowball:
                animations.loadFromDir("Assets/wand/snowball");
            break;
        }
        try {
            sf::Sprite sprite; sprite.setPosition(1000.0f,200.0f);
            animations.display(window,&sprite);
        }
        catch (std::exception &e) {
            std::cerr<<"Failed to load animations";
        }
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
    // std::cout<<"Nazwa Pliku to"<<filePath<<std::endl;
    file<<name<<space<<attackPoints<<space<<defensePoints<<space<<xp<<space<<maxHealth<<
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
    filePath>>name>>attackPoints>>defensePoints>>maxHealth;
    health=maxHealth;
    int type;
    filePath>>type;
    switch(type) {
        case 1:
            GameWidgets::oponentWeapon=new ShotGun();
        GameWidgets::oponentWeapon->setPosition(1200.0,800.0);
        break;
        case 2:
            GameWidgets::oponentWeapon=new Saber();
        GameWidgets::oponentWeapon->setPosition(1200.0,800.0);

        break;
        case 3:
            GameWidgets::oponentWeapon=new Wand();
        GameWidgets::oponentWeapon->setPosition(1200.0,800.0);

        break;
    }

}
void Fighter::saveData(std::string filePath) {
    const char space='\t';
    std::ofstream file(filePath);
    file<<name<<space<<attackPoints<<space<<defensePoints<<space<<space<<maxHealth;

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

}



#define gameW
//#define test


int main(int avg, char ** args) {

#ifdef gameW
    GameWidgets::load();
    Game game;
    game.startMenu();
    GameWidgets::release();
    return 0;
#endif





#ifdef test
    sf::RenderWindow win(sf::VideoMode(1000,1000),"test");
  //  Button * btn=new Button("Assets/AttackButton.jpg",200.0f,200.0f,true);
   // ShotGun * wpn=new ShotGun();
    //GameWidgets::load();
    sf::Sprite animationSprite;
    animationSprite.setPosition(100.0,100.0);
    Animations animations;
    animations.loadFight("Assets/player/");
    win.setFramerateLimit(60);
    bool once=true;
    PopOutWindow popOut("You have too little\n xp for this!");
    while (win.isOpen()) {
        sf::Event event;
        while (win.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                win.close();
        }
        win.clear();
        animations.displayAnimation(Animations::frames,&win,&animationSprite);
        win.display();
    }
    animations.release();
    //delete btn;
    //delete wpn;
#endif

}
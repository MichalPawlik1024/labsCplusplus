//
// Created by Michał on 21.03.2025.
//

#ifndef GAMEMECHANICWIDGETS_H
#define GAMEMECHANICWIDGETS_H
//#include "CrucialData.h"
//#define oldButton
//#define oldHealthBar
//#define oldDice
class Logger {
    public:
    static std::ofstream oLogger;
    static void writeMessage(std::string message) {
        oLogger<<message;
    }
    static void setLoggerFile(std::string filePath) {
        oLogger.open(filePath);
    }
};
std::ofstream Logger::oLogger("GameLogs.txt");
#ifdef oldHealthBar
class HealthBar
{
    sf::RectangleShape bar;
    sf::RenderWindow * master;
    int associatedValue;
    sf::Vector2f size;
public:
    HealthBar(sf::RenderWindow * master,sf::Vector2f size, sf::Vector2f pos, int assoc_val):master(master),
    size(size){
        bar.setSize(size);
        bar.setPosition(pos);
        bar.setFillColor(sf::Color::Green);
        this->associatedValue=assoc_val;
        size.x=assoc_val;
    }
    void draw() {
        master->draw(bar);
    }
    void decrement(int byValue) {
        if (size.x-float(byValue)>0) {
            size.x-=(float)byValue;
            bar.setSize(size);
        }
        else {
            size.x=0.1;
            bar.setSize(size);
            std::cerr<<"fatal decrementing";
        }

    }
    void reset() {
        size.x=associatedValue;
        bar.setSize(size);
    }
    void setAssociatedValue(int val) {
        associatedValue=val;
        size.x=associatedValue;
        bar.setSize(size);
    }
};

#endif

class HealthBar: BasicWidget {

    sf::RectangleShape bar;
    int associatedValue;
    sf::Vector2f size;
public:
    HealthBar(sf::Vector2f size, sf::Vector2f pos, int assoc_val):BasicWidget(),
    size(size){
        bar.setSize(size);
        bar.setPosition(pos);
        bar.setFillColor(sf::Color::Green);
        this->associatedValue=assoc_val;
        size.x=assoc_val;
    }
    virtual void draw(sf::RenderWindow * master) override {
        master->draw(bar);
    }
    void decrement(int byValue) {
        if (size.x-float(byValue)>0) {
            size.x-=(float)byValue;
            bar.setSize(size);
        }
        else {
            size.x=0.1;
            bar.setSize(size);
        }

    }
    void reset() {
        size.x=associatedValue;
        bar.setSize(size);
    }
    void setAssociatedValue(int val) {
        associatedValue=val;
        size.x=associatedValue;
        bar.setSize(size);
    }

};


#ifdef oldButton
class Button{
    sf::Texture  texture;
    sf::Sprite sprite;
    std::string displayName;
    sf::RenderWindow * master;

public:
    bool active;
    Button(std::string textureFile,std::string displayName,
        sf::RenderWindow * master,sf::Vector2f position, bool active=false ): texture(texture), displayName(displayName),master(master),active(active)

    {
        texture.loadFromFile(textureFile);
        sprite.setTexture(texture);
        sprite.setPosition(position);

    }
    void draw() const {
        if (active)
        master->draw(sprite);
    }
    bool isClicked() {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*master);
        sf::FloatRect bounds = sprite.getGlobalBounds();
        bool insideButton = bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        bool clicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        if (insideButton && clicked&&active) {
           // std::cout << "Button " << displayName << " clicked!!!" << std::endl;
            return true;
        }
        return false;
    }
};
#endif
class Button: public GraphicalObject {
    bool active;
public:
    Button(std::string textureFile,float posX, float posY, bool active):GraphicalObject(textureFile,posX,posY) {

        this->active=active;
    }
    ///Two incompatible logics, but easy to implement in code
    bool isActive() const {
        return active;
    }
    void activate() {
        active = true;
    }
    void deactivate() {
        active = false;
    }
    ///Change logic of active;

    virtual ~Button()  {}
};



#ifdef oldDice
class Dice {///TODO std::aray bo size jest staly
    std::vector<GraphicObject*> diceSides;

    sf::RenderWindow * master;
    int currentSide;
public:
    bool rolled=false;

    void setSide(int setNum) {
        currentSide=setNum;
    }
    Dice(){}
    Dice(sf::RenderWindow * master,sf::Vector2f pos):master(master) {
        currentSide=0;
        for (int i=1;i<7;i++) {
            std::string path="Assets/Dice";
            path+=std::to_string(i);
            path+=".jpg";
            diceSides.push_back(new GraphicObject(path,pos));

        }
        diceSides.push_back(new GraphicObject("Assets/EmptyDice.jpg",pos));
    }
    int roll();
    void draw() {
        diceSides.at(currentSide)->drawAt(master);
    }
    ~Dice() {
        for (auto & diceSide: diceSides) {
            delete diceSide;
        }
    }

};
#endif

class Dice: public GraphicalObject {///TODO std::aray bo size jest staly

    int currentSide;
public:
    void setSide(int setNum) {
        currentSide=setNum;
    }
    Dice(float posX, float posY):GraphicalObject("Assets/Dice1.jpg",posX,posY) {
    }
    int roll(sf::RenderWindow * master);

    virtual ~Dice() {}

};



//#define oldTextDisplayer
#ifdef oldTextDisplayer
class TextDisplayer {
public:
    sf::Font font;
    std::string textInfo;
    sf::RenderWindow * master;
    TextDisplayer( std::string fontFamilyFile,std::string textInfo="",
        unsigned int size = 30, sf::Color color = sf::Color::White):
    textInfo(textInfo)
    {
        font.loadFromFile(fontFamilyFile);
        text.setFont(font);
        text.setCharacterSize(size);
        text.setFillColor(color);
        text.setString(textInfo);
    }

    void setText(const std::string str) {
        textInfo=str;
        text.setString(textInfo);
    }

    void setPosition(float x, float y) {
        text.setPosition(x, y);
    }

    void setColor(const sf::Color& color) {
        text.setFillColor(color);
    }

    void draw() const {
        master->draw(text);
    }
    sf::FloatRect getGlobalBounds() {
        return text.getGlobalBounds();
    }

private:
    sf::Text text;
};
#endif

class TextDisplayer : public BasicWidget {
    std::string textInfo;
    sf::Font font;
    sf::Text text;
public:
    TextDisplayer( std::string fontFamilyFile,std::string textInfo="",
        unsigned int size = 30, sf::Color color = sf::Color::White, float posX=0.0f, float posY=0.0f):
    textInfo(textInfo)
    {
        font.loadFromFile(fontFamilyFile);
        text.setFont(font);
        text.setCharacterSize(size);
        text.setFillColor(color);
        text.setString(textInfo);
        setPosition(posX,posY);
    }

    TextDisplayer(int notAmbigous,nlohmann::json & jsonObj):textInfo(jsonObj["textInfo"]) {

        font.loadFromFile(jsonObj["fontFile"]);
        text.setFont(font);
        text.setCharacterSize(jsonObj["size"][0]);
        text.setFillColor(static_cast<sf::Color>(sf::Color::Red));
        text.setString(textInfo);
        setPosition(jsonObj["position"][0],jsonObj["position"][1]);

    }

    void setText(const std::string str) {
        textInfo=str;
        text.setString(textInfo);
    }

    void setPosition(float x, float y) {
        text.setPosition(x, y);
    }

    void setColor(const sf::Color& color) {
        text.setFillColor(color);
    }

    virtual void draw(sf::RenderWindow * master) override {
        master->draw(text);
    }
    sf::FloatRect getGlobalBounds() {
        return text.getGlobalBounds();
    }
    std::string getText() {
        return textInfo;
    }
    virtual ~TextDisplayer() {}
};
class Weapon;
class FigthResultDisplayer {
private:
    TextDisplayer textDisplayer;
    bool result;
public:
    FigthResultDisplayer(float posX=400.f, float posY=400.f,sf::Color color=sf::Color::Green): result(true), textDisplayer("Assets/minecraft/Minecraft.ttf") {
        textDisplayer.setPosition(posX,posY);
        textDisplayer.setColor(color);

    }
    void drawResult(bool whoWon, sf::RenderWindow * master) {
        result=whoWon;
        if (whoWon) {
            textDisplayer.setText("You won");
            textDisplayer.draw(master);
        }
        else {
            textDisplayer.draw(master);
            textDisplayer.setText("You lost");
        }
    }
    void draw(sf::RenderWindow * master) {
        if (result) {
            textDisplayer.setText("You won");
        }
        else {
            textDisplayer.setText("You lost");
        }
        textDisplayer.draw(master);
    }
};




#define oldPlayer

#ifdef oldPlayer
class OldPlayer : public GraphicalObject {
    std::string name;
    int attackPoints;
    int defensePoints;
    int xpPoints;
    int money;
    int health;
    int maxHealth;
public:
    int getMaxHealth()  {
        return maxHealth;
    }
    std::string getHealth() {
        return std::to_string(health);
    }
    std::string getAttack() {
        return std::to_string(attackPoints);
    }
    std::string getDefence() {
        return std::to_string(defensePoints);
    }
    std::string getMoney() {
        return std::to_string(money);
    }
    std::string getXp() {
        return std::to_string(xpPoints);
    }
    void incrementHealth() {
        maxHealth++;
    }
    void incrementAttack() {
        attackPoints++;
    }
    void incrementDefense() {
        defensePoints++;
    }
    void resetStats() {
        health=maxHealth;
    }

    bool visible;
     int id;
    ///TODO constructor logic, loading this window assets in private method
    ///Adding buttons for two stats maybe more
    OldPlayer(std::string name="", std::string skinAssetPath="",
        int attackPoints = 1, int defensePoints = 1, int xpPoints = 0,
        int money = 0, int health = 100, int maxHealth=100, int id=0,bool visible=false,
        float posX=0.0f, float posY=0.0f):
    attackPoints(attackPoints),defensePoints(defensePoints),xpPoints(xpPoints),money(money),health(health),
    maxHealth(maxHealth),id(id),name(name),visible(visible),GraphicalObject(skinAssetPath,posX,posY) {
    }
    OldPlayer(std::ifstream & filePath,std::string skinAssetPath="", float posX=0.0, float posY=0.0);

    void decrementMoney(int amount) {
        money-=amount;
    }
    void decrementXp(int amount) {
        xpPoints-=amount;
        assert(xpPoints>=0);
    }
    bool fight(OldPlayer * opponent,int attackFactor );

    void saveData(std::string  filePath);
    ~OldPlayer() {

    }
};
#endif

///
///TODO koniecznie dodac logike wczytywania i zapisywania levelu Fightera
///
class Fighter : public GraphicalObject {
private:
    int level{0};
    int maxAttack{5};
    int maxDefense{5};
protected:
    std::string name;
    int attackPoints;
    int defensePoints;
    int health;
    int maxHealth;
public:
    int getMaxHealth()  {
        return maxHealth;
    }
    std::string getHealth() {
        return std::to_string(health);
    }
    std::string getAttack() {
        return std::to_string(attackPoints);
    }
    std::string getDefence() {
        return std::to_string(defensePoints);
    }
    int getHealthI() {
        return health;
    }
    int getAttackI() {
        return attackPoints;
    }
    int getDefenceI() {
        return defensePoints;
    }
    void incrementHealth() {
        maxHealth++;
    }
    void incrementAttack() {
        attackPoints++;
    }
    void incrementDefense() {
        defensePoints++;
    }
    void resetStats() {
        health=maxHealth;
    }
    void reduceHealth(int amount) {
        health-=amount;
    }
    void setAttack(int attack) {
        attackPoints=attack;
    }
    void setDefense(int defense) {
        defensePoints=defense;
    }
    void setMaxHealth(int maxHealth) {
        this->maxHealth=maxHealth;
    }
    void setHealth(int health) {
        this->health=health;
    }

    bool visible;
    int id;
    ///TODO constructor logic, loading this window assets in private method
    ///Adding buttons for two stats maybe more
    Fighter(std::string name="", std::string skinAssetPath="",
        int attackPoints = 1, int defensePoints = 1, int health = 100, int maxHealth=100, int id=0,bool visible=false,
        float posX=0.0f, float posY=0.0f):
    attackPoints(attackPoints),defensePoints(defensePoints),health(health),
    maxHealth(maxHealth),id(id),name(name),visible(visible),GraphicalObject(skinAssetPath,posX,posY) {
    }
    Fighter(std::ifstream & filePath,std::string skinAssetPath="Assets/Opponents/Major.jpg", float posX=0.0, float posY=0.0);


    virtual bool fight(Fighter * opponent, int attackFactor);

    ~Fighter() {

    }

    virtual void saveData(std::string  filePath);
    bool selfUpgrade() {
        if (attackPoints<maxAttack) {
            attackPoints++;
        }
        if (defensePoints<maxDefense) {
            defensePoints++;
        }
        return attackPoints<maxAttack&&defensePoints<maxDefense;//W przeciwnym wypadku nie mozna dalej upgradowac
    }
    void incrementLevel(){
        level++;
        }

    void setMaxAttack(int amount) {
            maxAttack=amount;
    }
    void setMaxDefense(int amount) {
        maxDefense=amount;
    }
    int getLevel() {
        return level;
    }
};

class Player : public Fighter {

    int money;
    int xp;
public:
    Player(std::string name,std::string skinAssetPath,int attackPoints = 1, int defensePoints = 1, int health = 100, int maxHealth=100, int id=0,bool visible=false,
        float posX=0.0f, float posY=0.0f, int money=0, int xp=0):Fighter(name,skinAssetPath,attackPoints,defensePoints,
            health,maxHealth,id,visible,posX,posY),money(money),xp(xp){
            }
    bool fight(Fighter * opponent, int attackFactor) override;
    Player(std::ifstream & filePath,std::string skinAssetPath="", float posX=0.0, float posY=0.0);


    std::string getMoney() {
        return std::to_string(money);
    }
    void decrementMoney(int amount) {
        money-=amount;
    }
    void decrementXp(int amount) {
        xp-=amount;
        assert(xpPoints>=0);
    }
    std::string getXp() {
        return std::to_string(xp);
    }
    void saveData(std::string  filePath) override;

    ~Player() {}
};







class Scene : public GraphicalObject{
public:
    Scene(std::string assetFile):GraphicalObject(assetFile,0.0f,0.0f) {
    }
    virtual ~Scene() {}
};
/*
 void Player::saveData(std::string  filePath)  {
    const char space='\t';
    std::ofstream file(filePath);
   // std::cout<<"Nazwa Pliku to"<<filePath<<std::endl;
    file<<name<<space<<attackPoints<<space<<defensePoints<<space<<xpPoints<<space<<maxHealth<<
        space<<money;

}
*/

class AnimationsLibrary {
public:
    std::vector<sf::Texture*> frames;
    AnimationsLibrary()=default;
    void loadFromDir(std::string directory) {
        try {
            for (auto & entry: std::filesystem::directory_iterator(directory)) {
                try {
                    sf::Image img;
                    img.loadFromFile(entry.path().string());
                    img.createMaskFromColor(sf::Color::Black);
                    frames.emplace_back(new sf::Texture());
                    //frames.back()->loadFromFile(entry.path().string());
                    frames.back()->loadFromImage(img);
                }
                catch (std::exception & e) {
                    std::cout << e.what() << std::endl;
                }

            }
        }
        catch (std::exception & e) {}

    }
    void display(sf::RenderWindow * window, sf::Sprite * where);
    void release() {
        for (auto & frame:frames)
            delete frame;
    }
};



enum class WeaponType {
     shotgun, wand, saber, basic
 };
class Weapon: public GraphicalObject {
protected:


    public:
     Weapon(std::string assetPath):GraphicalObject(assetPath,400.0,800.0) {

     }
     virtual WeaponType getType() {
         return WeaponType::basic;
     }
#ifdef oldPlayer
    virtual int getAttackFactor(OldPlayer * player){return 0;}
    virtual int getDefenseFactor(OldPlayer * player){return 0;}
#endif
    virtual int getAttackFactor(Fighter * player){return 0;}
    virtual int getDefenseFactor(Fighter * player){return 0;}

    virtual void attackAnimation(sf::RenderWindow * window, int attacker=0) {return;}
    virtual std::string getInfo() {
         return "Basic weapon\n A classic\n Aim well ";
     }

     ///
     /// @return  false->no further upgrade possible, true->further upgrade possible
     virtual bool selfUpgrade() {
         return false;
     }
    virtual nlohmann::json getJson() {
         return nlohmann::json();
     }
    virtual void loadFromJson(nlohmann::json & json) {

     }
    //virtual ~Weapon();
 };
class ShotGun:public Weapon {

    public:
    ShotGun():Weapon("Assets/shotgun.jpg") {
        this->setMask(sf::Color::White);

    }
#ifdef oldPlayer
    virtual int getAttackFactor(OldPlayer *player) override {
        ///shot can be precise or can be not precise
       return std::stoi(player->getAttack())/10*(rand()%2);
    }
    virtual int getDefenseFactor(OldPlayer *player) override {
        return 0;
    }
#endif
    virtual int getAttackFactor(Fighter *player) override {
        ///shot can be precise or can be not precise
        return std::stoi(player->getAttack())/10*(rand()%2);
    }
    virtual int getDefenseFactor(Fighter *player) override {
        return 0;
    }

    WeaponType getType() override {
        return WeaponType::shotgun;
    }
    void attackAnimation(sf::RenderWindow * window, int attacker=0) override ;
    std::string getInfo() override {
        return "Basic weapon\n A classic\n Aim well ";
    }
    nlohmann::json getJson() override {
        return nlohmann::json();
    }
    void loadFromJson(nlohmann::json & json) override {

    }

//virtual ~ShotGun(){}
};
class Saber:public Weapon {
    int sharpnessLevel;
    public:
    Saber():Weapon("Assets/saber.jpg") {

        sharpnessLevel=1;
        this->setMask(sf::Color(139,177,207));

    }
#ifdef oldPlayer
    virtual int getAttackFactor(OldPlayer *player) override {
        ///chance of smash attac
        return 10*(rand()%2);
    }
    virtual int getDefenseFactor(OldPlayer *player) override {
        return 10;
    }
#endif
    virtual int getAttackFactor(Fighter *player) override {
        ///chance of smash attac
        return 10*(rand()%2);
    }
    virtual int getDefenseFactor(Fighter *player) override {
        return 10;
    }



    WeaponType getType() override {
        return WeaponType::saber;
    }
    virtual void attackAnimation(sf::RenderWindow *window, int attacker=0) override;
    std::string getInfo() override {
        return "Elegant weapon\n For more civilized times\n Sharpness "+std::to_string(sharpnessLevel);
    }
    bool selfUpgrade() override {
        if (sharpnessLevel<5) {
            sharpnessLevel++;
            return true;
        }
        else {
            return false;
        }
    }
    nlohmann::json getJson() override {
        nlohmann::json result;
        result["sharpnessLevel"] = sharpnessLevel;
        return result;
    }
    void loadFromJson(nlohmann::json & json) override {
        sharpnessLevel = json["sharpnessLevel"];
    }
};

enum class Spell{ fireball, snowball, shield};
class Wand: public Weapon {
    Spell currentSpell;
    public:
    Wand():Weapon("Assets/wand.jpg") {
        currentSpell=Spell::fireball;
    }
    //virtual void attackAnimation(sf::RenderWindow * window);
    std::string getInfo() override {
        switch(currentSpell) {
        case Spell::fireball:
        return "Powerful magical \n weapon \n current spell: \n fireball";
        case Spell::snowball:
        return "Powerful magical \n weapon \n current spell: \n snowball";
        case Spell::shield:
        return "Powerful magical \n weapon \n current spell: \n shield";
        default:
        return "Your wand\n is broken\n you messed with \n json motherfucker";
        }
    }
    WeaponType getType() override {
        return WeaponType::wand;
    }
    bool selfUpgrade() override {
        switch (currentSpell) {
            case Spell::fireball:
                currentSpell=Spell::snowball;
            return true;
            case Spell::snowball:
                currentSpell=Spell::shield;
            return true;
            case Spell::shield:
                return false;
        }
    }
    nlohmann::json getJson() override {
        nlohmann::json result;
        result["currentSpell"] = currentSpell;
        return result;
    }
    void loadFromJson(nlohmann::json & json) override {
        currentSpell = json["currentSpell"];
    }
    void attackAnimation(sf::RenderWindow *window, int attacker=0) override;
};




class WeaponUpgradeCenter {
    WeaponType current;
public:
    static std::map<WeaponType,int> prices;
    WeaponUpgradeCenter(WeaponType  currentWeaponType):current(currentWeaponType) {

    }
#ifdef oldPlayer
    Weapon * newWeapon(Weapon * weapon, OldPlayer * player);
#endif
    Weapon * newWeapon(Weapon * weapon, Player * player);
};
/// Structure of this array is [nextWeaponPrice, nextWeaponPrice]
/// So everything is hardcoded like that
std::map<WeaponType,int> WeaponUpgradeCenter::prices
{{WeaponType::shotgun,200},{WeaponType::saber,400},{WeaponType::wand,300}};

class EventBinder {
    std::unordered_map<BasicWidget *,std::function<void()>> bindedActions;
public:
    EventBinder() {
    }
    void handleAction(BasicWidget * widget) {
        try {
            bindedActions[widget]();
        }
        catch (std::exception & e) {
            ///If there is an action that is not yet asigned ignore it
        }
    }
    void bindAction(BasicWidget * widget, std::function<void()> action) {
        bindedActions[widget] = action;
    }
    ~EventBinder() {}
};


class OpponentUpgradeCenter {



public:
    static std::array<std::string,5> charactersTexturePaths;
    static std::vector<std::vector<int>> moneyAndXpThresholds;
    static std::array<std::string,5> charactersNames;
    static std::vector<sf::Texture*> charactersTextures;
    void newOpponent(Fighter * current, Player * player) ;
};

    std::array<std::string,5> OpponentUpgradeCenter::charactersNames{
    "Major","Shreder","Shrek","Osiol","Chuck Norris"};
    std::array<std::string,5> OpponentUpgradeCenter::charactersTexturePaths{
      "Assets/Opponents/Major.jpg",
        "Assets/Opponents/Shreder.jpg",
        "Assets/Opponents/Shrek.jpg",
            "Assets/Opponents/Osiol.jpg",
        "Assets/Opponents/ChuckNorris.jpg"
    };
    std::vector<std::vector<int>> OpponentUpgradeCenter::moneyAndXpThresholds
    {
    {200,20},{300,30},{400,40},{500,50}
    };

    std::vector<sf::Texture*> OpponentUpgradeCenter::charactersTextures{};




class DefaultButton : public TextDisplayer{
    //TextDisplayer buttonText;
    sf::RectangleShape outline;
    sf::Vector2f size;
public:
    DefaultButton(std::string displayedText,double sizeX, double sizeY,double posX,
        double posY,sf::Color backgroundColor=sf::Color::Green, sf::Color fontColor=sf::Color::Black):
    TextDisplayer("Assets/Minecraft/minecraft.ttf",displayedText,50,
        sf::Color::White,
            posX,posY) {
        setText(displayedText);
        setColor(fontColor);
        sf::FloatRect textBounds = TextDisplayer::getGlobalBounds();
        if (sizeY < 30) sizeY = 50;
        if (sizeX < textBounds.width + 40) sizeX = textBounds.width + 40;
        size = sf::Vector2f(sizeX, sizeY);
        outline.setSize(size);
        outline.setFillColor(backgroundColor);
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

#endif //GAMEMECHANICWIDGETS_H

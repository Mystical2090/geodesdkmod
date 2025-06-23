#include <Geode/Geode.hpp>

using namespace geode::prelude;
// everything seperate file
#include <Geode/modify/MenuLayer.hpp>

#include <Geode/modify/PlayLayer.hpp>

#include <Geode/modify/LevelInfoLayer.hpp>

#include <Geode/modify/CreatorLayer.hpp>

#include <random>

class ModSettings : public CCObject {
public:
    static ModSettings* getInstance() {
        static ModSettings* instance = nullptr;
        if (!instance) {
            instance = new ModSettings();
        }
        return instance;
    }
    
    bool pranksEnabled = true;
    bool customMessages = true;
    bool uiModifications = true;
    float progressBarX = 120.0f;
    float percentageLabelX = 231.0f;
};

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        
        auto customBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png"),
            this,
            menu_selector(MyMenuLayer::onCustomButton)
        );
        customBtn->setPosition({-200, -100});
        customBtn->setScale(0.8f);
        
        auto customLabel = CCLabelBMFont::create("Mod Settings", "bigFont.fnt");
        customLabel->setScale(0.4f);
        customLabel->setPosition(customBtn->getContentSize() / 2);
        customBtn->addChild(customLabel);
        
        this->getChildByID("main-menu")->addChild(customBtn);
        
        return true;
    }
    
    void onPlay(CCObject* sender) {
        if (!ModSettings::getInstance()->pranksEnabled) {
            MenuLayer::onPlay(sender);
            return;
        }
        
        std::vector<std::string> prankMessages = {
            "hA you <cp>cant</c> play the level!",
            "Nice try, but <cy>nope</c>!",
            "The level is <cr>temporarily closed</c>!",
            "Come back <cg>later</c> maybe?",
            "Level machine is <co>broken</c>!"
        };
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, prankMessages.size() - 1);
        
        FLAlertLayer::create("Sike!", 
            prankMessages[dis(gen)], 
            "Aww man"
        )->show();
    }
    
    void onOptions(CCObject* sender) {
        if (!ModSettings::getInstance()->pranksEnabled) {
            MenuLayer::onOptions(sender);
            return;
        }
        
        std::vector<std::string> optionMessages = {
            "Frick you you cant change it.",
            "<cr>Options are locked</c> buddy!",
            "Settings are <cy>off limits</c>!",
            "No touchy the <cg>options</c>!",
            "Access <co>denied</c>!"
        };
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, optionMessages.size() - 1);
        
        FLAlertLayer::create("Sike!", 
            optionMessages[dis(gen)], 
            "Fine..."
        )->show();
    }
    
    void onCustomButton(CCObject* sender) {
        auto settings = ModSettings::getInstance();
        
        auto alert = FLAlertLayer::create(
            "Mod Settings",
            fmt::format("Pranks: {}\nUI Mods: {}\nCustom Messages: {}",
                settings->pranksEnabled ? "ON" : "OFF",
                settings->uiModifications ? "ON" : "OFF",
                settings->customMessages ? "ON" : "OFF"),
            "OK"
        );
        
        alert->show();
    }
    
    void togglePranks(CCObject* sender) {
        ModSettings::getInstance()->pranksEnabled = !ModSettings::getInstance()->pranksEnabled;
        
        FLAlertLayer::create("Success!", 
            ModSettings::getInstance()->pranksEnabled ? 
                "Pranks <cg>enabled</c>!" : "Pranks <cr>disabled</c>!",
            "OK"
        )->show();
    }
    
    void toggleUI(CCObject* sender) {
        ModSettings::getInstance()->uiModifications = !ModSettings::getInstance()->uiModifications;
        
        FLAlertLayer::create("Success!", 
            ModSettings::getInstance()->uiModifications ? 
                "UI modifications <cg>enabled</c>!" : "UI modifications <cr>disabled</c>!",
            "OK"
        )->show();
    }
};

class $modify(MyPlayLayer, PlayLayer) {
    int m_attemptClickCount = 0;
    
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        
        if (ModSettings::getInstance()->uiModifications) {
            if (auto progressBar = this->getChildByID("progress-bar")) {
                progressBar->setPositionX(ModSettings::getInstance()->progressBarX);
            }
            
            if (auto percentageLabel = this->getChildByID("percentage-label")) {
                percentageLabel->setPositionX(ModSettings::getInstance()->percentageLabelX);
            }
            
            this->schedule(schedule_selector(MyPlayLayer::checkCompletion), 0.1f);
        }
        
        return true;
    }
    
    void checkCompletion(float dt) {
        if (this->m_hasCompletedLevel && ModSettings::getInstance()->customMessages) {
            this->unschedule(schedule_selector(MyPlayLayer::checkCompletion));
            this->runAction(CCSequence::create(
                CCDelayTime::create(1.0f),
                CCCallFunc::create(this, callfunc_selector(MyPlayLayer::showCustomCompletion)),
                nullptr
            ));
        }
    }
    
    void showCustomCompletion() {
        std::vector<std::string> completionMessages = {
            "Wow! <cy>You actually did it</c>!",
            "<cg>Impressive</c> work there!",
            "Level <cp>conquered</c>!",
            "You're <co>on fire</c> today!",
            "<cr>Beast mode</c> activated!"
        };
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, completionMessages.size() - 1);
        
        FLAlertLayer::create("GG!", 
            completionMessages[dis(gen)], 
            "Thanks!"
        )->show();
    }
    
    void onQuit() {
        if (ModSettings::getInstance()->pranksEnabled && this->m_attemptClickCount > 10) {
            std::vector<std::string> quitMessages = {
                "Giving up <cr>already</c>?",
                "Just <cy>one more try</c>!",
                "Don't <co>quit now</c>!",
                "You were <cg>so close</c>!",
                "<cp>Persistence</c> is key!"
            };
            
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, quitMessages.size() - 1);
            
            FLAlertLayer::create("Wait!", 
                quitMessages[dis(gen)], 
                "OK"
            )->show();
            return;
        }
        
        PlayLayer::onQuit();
    }
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        
        if (ModSettings::getInstance()->customMessages) {
            this->schedule(schedule_selector(MyLevelInfoLayer::addCustomInfo), 0.1f);
        }
        
        return true;
    }
    
    void addCustomInfo(float dt) {
        this->unschedule(schedule_selector(MyLevelInfoLayer::addCustomInfo));
        
        std::map<int, std::string> difficultyMemes = {
            {0, "Auto: <cg>Chill mode</c>"},
            {1, "Easy: <cy>Still easy</c>"},
            {2, "Normal: <co>Getting spicy</c>"},
            {3, "Hard: <cp>Oof moment</c>"},
            {4, "Harder: <cr>Pain incoming</c>"},
            {5, "Insane: <cr>RIP fingers</c>"},
            {6, "Demon: <cr>Good luck!</c>"}
        };
        
        auto difficulty = static_cast<int>(this->m_level->m_difficulty);
        auto customLabel = CCLabelBMFont::create(
            difficultyMemes.count(difficulty) ? 
                difficultyMemes[difficulty].c_str() : 
                "Unknown: <cy>Mystery level</c>",
            "chatFont.fnt"
        );
        
        customLabel->setPosition({CCDirector::sharedDirector()->getWinSize().width / 2, 50});
        customLabel->setScale(0.8f);
        this->addChild(customLabel, 100);
    }
};

class $modify(MyCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;
        
        if (ModSettings::getInstance()->pranksEnabled) {
            this->schedule(schedule_selector(MyCreatorLayer::addPrankListeners), 0.1f);
        }
        
        return true;
    }
    
    void addPrankListeners(float dt) {
        this->unschedule(schedule_selector(MyCreatorLayer::addPrankListeners));
        
        if (auto createBtn = this->getChildByID("create-button")) {
            if (auto menuItem = typeinfo_cast<CCMenuItemSpriteExtra*>(createBtn)) {
                menuItem->setTarget(this, menu_selector(MyCreatorLayer::onPrankCreate));
            }
        }
    }
    
    void onPrankCreate(CCObject* sender) {
        std::vector<std::string> createMessages = {
            "Create button is <cr>out of order</c>!",
            "Editor is <cy>temporarily closed</c>!",
            "Come back when you're <cg>more creative</c>!",
            "Creation mode <co>locked</c>!",
            "Try again <cp>tomorrow</c>!"
        };
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, createMessages.size() - 1);
        
        FLAlertLayer::create("Oops!", 
            createMessages[dis(gen)], 
            "Damn"
        )->show();
    }
};

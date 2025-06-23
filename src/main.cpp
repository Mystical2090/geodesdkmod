#include <Geode/Geode.hpp>
// shi 
using namespace geode::prelude;
//s hit
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
        FLAlertLayer::create("No!", "You can't do that!", "OK")->show();
    }
    
    void onOptions(CCObject* sender) {
        FLAlertLayer::create("No!", "You can't do that!", "OK")->show();
    }
    
    void onCustomButton(CCObject* sender) {
        FLAlertLayer::create("No!", "You can't do that!", "OK")->show();
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
    struct Fields {
        int m_attemptClickCount = 0;
    };
    
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
        FLAlertLayer::create("No!", "You can't do that!", "OK")->show();
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
        FLAlertLayer::create("No!", "You can't do that!", "OK")->show();
    }
};

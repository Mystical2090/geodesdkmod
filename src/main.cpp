#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/PlayLayer.hpp>

#include <Geode/modify/PlayerObject.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

#include <Geode/modify/GameManager.hpp>

#include <Geode/modify/MenuLayer.hpp>

#include <Geode/modify/LevelInfoLayer.hpp>

#include <Geode/modify/GameStatsManager.hpp>

class $modify(MyPlayerObject, PlayerObject) {
    bool init(int p0, int p1, GJBaseGameLayer* p2, cocos2d::CCLayer* p3, bool p4) {
        if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;
        this->applyMiniMeEffects();
        this->applyRainbowTrail();
        return true;
    }

    void update(float dt) {
        PlayerObject::update(dt);
        this->applyMiniMeEffects();
        this->updateRainbowTrail();

        float scale = Mod::get()->getSettingValue<double>("player-scale");
        if (scale <= 0.3f && Mod::get()->getSettingValue<bool>("enable-mini-trail")) {
            this->addMiniTrail();
        }

        if (Mod::get()->getSettingValue<bool>("enable-auto-jump")) {
            this->handleAutoJump();
        }

        if (Mod::get()->getSettingValue<bool>("enable-custom-gravity")) {
            this->applyCustomGravity();
        }
    }

    void applyMiniMeEffects() {
        float scale = Mod::get()->getSettingValue<double>("player-scale");
        this->setScale(scale);

        float jumpBoost = Mod::get()->getSettingValue<double>("jump-boost");
        if (scale < 1.0f && Mod::get()->getSettingValue<bool>("enable-jump-boost")) {
            auto pos = this->getPosition();
            pos.y += jumpBoost;
            this->setPosition(pos);
        }

        if (Mod::get()->getSettingValue<bool>("enable-transparency")) {
            float opacity = Mod::get()->getSettingValue<double>("player-opacity");
            this->setOpacity(static_cast<GLubyte>(opacity * 255));
        }
    }

    void addMiniTrail() {
        if (rand() % 10 == 0) {
            auto particle = cocos2d::CCParticleSystemQuad::create();
            if (particle) {
                auto texture = cocos2d::CCTextureCache::sharedTextureCache()->addImage("square02_001.png", false);
                if (texture) particle->setTexture(texture);

                particle->setPosition(this->getPosition());
                particle->setScale(0.1f);
                particle->setLife(0.5f);
                particle->setStartColor({1.0f, 1.0f, 0.0f, 1.0f});
                particle->setEndColor({1.0f, 0.0f, 0.0f, 0.0f});
                particle->setTotalParticles(1);

                if (this->getParent()) {
                    this->getParent()->addChild(particle);
                    particle->runAction(cocos2d::CCSequence::create(
                        cocos2d::CCDelayTime::create(0.5f),
                        cocos2d::CCRemoveSelf::create(),
                        nullptr
                    ));
                }
            }
        }
    }

    void applyRainbowTrail() {
        if (Mod::get()->getSettingValue<bool>("enable-rainbow-trail")) {
            static float hue = 0.0f;
            hue += 0.01f;
            if (hue > 1.0f) hue = 0.0f;
            
            cocos2d::ccColor3B rainbowColor = cocos2d::ccc3(
                255 * (0.5f + 0.5f * cosf(hue * 6.28f)),
                255 * (0.5f + 0.5f * cosf(hue * 6.28f + 2.09f)),
                255 * (0.5f + 0.5f * cosf(hue * 6.28f + 4.18f))
            );
            this->setColor(rainbowColor);
        }
    }

    void updateRainbowTrail() {
        if (Mod::get()->getSettingValue<bool>("enable-rainbow-trail") && rand() % 5 == 0) {
            auto trail = cocos2d::CCSprite::createWithSpriteFrameName("square02_001.png");
            if (trail && this->getParent()) {
                trail->setPosition(this->getPosition());
                trail->setScale(0.2f);
                trail->setColor(this->getColor());
                trail->setOpacity(150);
                this->getParent()->addChild(trail);
                
                trail->runAction(cocos2d::CCSequence::create(
                    cocos2d::CCSpawn::create(
                        cocos2d::CCFadeOut::create(1.0f),
                        cocos2d::CCScaleBy::create(1.0f, 0.1f),
                        nullptr
                    ),
                    cocos2d::CCRemoveSelf::create(),
                    nullptr
                ));
            }
        }
    }

    void handleAutoJump() {
        if (this->m_isOnGround && rand() % 30 == 0) {
            this->pushButton(PlayerButton::Jump);
        }
    }

    void applyCustomGravity() {
        float gravityMultiplier = Mod::get()->getSettingValue<double>("gravity-multiplier");
        this->m_gravityMod = gravityMultiplier;
    }

    void playerDestroyed(bool p0) {
        PlayerObject::playerDestroyed(p0);

        float scale = Mod::get()->getSettingValue<double>("player-scale");
        if (scale <= 0.5f && Mod::get()->getSettingValue<bool>("enable-mini-death-effect")) {
            this->createMiniDeathEffect();
        }

        if (Mod::get()->getSettingValue<bool>("enable-enhanced-death-effect")) {
            this->createEnhancedDeathEffect();
        }
    }

    void createMiniDeathEffect() {
        for (int i = 0; i < 5; i++) {
            auto spark = cocos2d::CCSprite::createWithSpriteFrameName("d_effect_01_001.png");
            if (spark && this->getParent()) {
                spark->setPosition(this->getPosition());
                spark->setScale(0.3f);
                spark->setColor({255, 255, 0});
                this->getParent()->addChild(spark);

                float angle = (rand() % 360) * M_PI / 180.0f;
                float distance = 50.0f + (rand() % 50);

                auto moveAction = cocos2d::CCMoveBy::create(0.3f, {
                    cosf(angle) * distance,
                    sinf(angle) * distance
                });

                spark->runAction(cocos2d::CCSequence::create(
                    cocos2d::CCSpawn::create(
                        moveAction,
                        cocos2d::CCFadeOut::create(0.3f),
                        cocos2d::CCScaleBy::create(0.3f, 0.1f),
                        nullptr
                    ),
                    cocos2d::CCRemoveSelf::create(),
                    nullptr
                ));
            }
        }
    }

    void createEnhancedDeathEffect() {
        if (this->getParent()) {
            this->getParent()->runAction(cocos2d::CCSequence::create(
                cocos2d::CCMoveBy::create(0.05f, {5, 0}),
                cocos2d::CCMoveBy::create(0.05f, {-10, 0}),
                cocos2d::CCMoveBy::create(0.05f, {5, 0}),
                nullptr
            ));
        }

        for (int i = 0; i < 15; i++) {
            auto particle = cocos2d::CCSprite::createWithSpriteFrameName("square02_001.png");
            if (particle && this->getParent()) {
                particle->setPosition(this->getPosition());
                particle->setScale(0.15f);
                particle->setColor({255, static_cast<GLubyte>((rand() % 100) + 155), 0});
                this->getParent()->addChild(particle);

                float angle = (rand() % 360) * M_PI / 180.0f;
                float distance = 80.0f + (rand() % 100);

                particle->runAction(cocos2d::CCSequence::create(
                    cocos2d::CCSpawn::create(
                        cocos2d::CCMoveBy::create(0.5f, {
                            cosf(angle) * distance,
                            sinf(angle) * distance
                        }),
                        cocos2d::CCFadeOut::create(0.5f),
                        cocos2d::CCRotateBy::create(0.5f, 360),
                        nullptr
                    ),
                    cocos2d::CCRemoveSelf::create(),
                    nullptr
                ));
            }
        }
    }
};

class $modify(Speedhack, GJBaseGameLayer) {
public:
    void update(float dt) {
        if (Mod::get()->getSettingValue<bool>("enable-speed-boost")) {
            float multiplier = Mod::get()->getSettingValue<float>("speed-multiplier");
            GJBaseGameLayer::update(dt * multiplier);
        } else {
            GJBaseGameLayer::update(dt);
        }

        if (Mod::get()->getSettingValue<bool>("enable-frame-step")) {
            static bool stepping = false;
            if (!stepping) {
                stepping = true;
            }
        }
    }
};

class $modify(EnhancedPlayLayer, PlayLayer) {
public:
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        
        this->setupToolkitFeatures();
        return true;
    }

    void setupToolkitFeatures() {
        if (Mod::get()->getSettingValue<bool>("show-fps")) {
            this->addFPSDisplay();
        }

        if (Mod::get()->getSettingValue<bool>("show-attempt-counter")) {
            this->addAttemptCounter();
        }

        if (Mod::get()->getSettingValue<bool>("show-progress-bar")) {
            this->addProgressBar();
        }
    }

    void addFPSDisplay() {
        auto fpsLabel = cocos2d::CCLabelBMFont::create("FPS: 60", "goldFont.fnt");
        if (fpsLabel) {
            fpsLabel->setPosition({50, this->getContentSize().height - 30});
            fpsLabel->setScale(0.5f);
            fpsLabel->setTag(1001);
            this->addChild(fpsLabel, 100);
        }
    }

    void addAttemptCounter() {
        auto attemptLabel = cocos2d::CCLabelBMFont::create("Attempt: 1", "goldFont.fnt");
        if (attemptLabel) {
            attemptLabel->setPosition({this->getContentSize().width - 100, this->getContentSize().height - 30});
            attemptLabel->setScale(0.5f);
            attemptLabel->setTag(1002);
            this->addChild(attemptLabel, 100);
        }
    }

    void addProgressBar() {
        auto progressBG = cocos2d::CCSprite::createWithSpriteFrameName("GJ_progressBar_001.png");
        auto progressFill = cocos2d::CCSprite::createWithSpriteFrameName("GJ_progressBar_001.png");
        
        if (progressBG && progressFill) {
            progressBG->setPosition({this->getContentSize().width / 2, 30});
            progressBG->setScale(0.8f);
            progressBG->setOpacity(100);
            this->addChild(progressBG, 100);

            progressFill->setPosition({this->getContentSize().width / 2, 30});
            progressFill->setScale(0.8f);
            progressFill->setColor({0, 255, 0});
            progressFill->setTag(1003);
            this->addChild(progressFill, 101);
        }
    }

    void update(float dt) {
        PlayLayer::update(dt);
        
        if (auto fpsLabel = static_cast<cocos2d::CCLabelBMFont*>(this->getChildByTag(1001))) {
            static int frameCount = 0;
            static float timeAccum = 0;
            frameCount++;
            timeAccum += dt;
            
            if (timeAccum >= 1.0f) {
                int fps = frameCount;
                fpsLabel->setString(("FPS: " + std::to_string(fps)).c_str());
                frameCount = 0;
                timeAccum = 0;
            }
        }
        
        if (auto progressFill = this->getChildByTag(1003)) {
            float progress = this->getCurrentPercent() / 100.0f;
            progressFill->setScaleX(0.8f * progress);
        }

        if (Mod::get()->getSettingValue<bool>("enable-instant-restart")) {
            this->handleInstantRestart();
        }
    }

    void handleInstantRestart() {
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (!Mod::get()->getSettingValue<bool>("noclip")) {
            PlayLayer::destroyPlayer(player, object);

            if (auto attemptLabel = static_cast<cocos2d::CCLabelBMFont*>(this->getChildByTag(1002))) {
                static int attempts = 0;
                attempts++;
                attemptLabel->setString(("Attempt: " + std::to_string(attempts)).c_str());
            }
        }
    }

    void togglePracticeMode(bool practice) {
        PlayLayer::togglePracticeMode(practice);
        
        if (Mod::get()->getSettingValue<bool>("enhanced-practice-mode")) {
            this->setupEnhancedPracticeMode(practice);
        }
    }

    void setupEnhancedPracticeMode(bool practice) {
        if (practice) {
            auto checkpointLabel = cocos2d::CCLabelBMFont::create("Checkpoints: 0", "goldFont.fnt");
            if (checkpointLabel) {
                checkpointLabel->setPosition({150, this->getContentSize().height - 30});
                checkpointLabel->setScale(0.5f);
                checkpointLabel->setTag(1004);
                this->addChild(checkpointLabel, 100);
            }
        }
    }
};

class $modify(MyGameStatsManager, GameStatsManager) {
public:
    void completedLevel(GJGameLevel* level) {
        if (Mod::get()->getSettingValue<bool>("track-level-stats")) {
            this->trackLevelCompletion(level);
        }
    }

    void trackLevelCompletion(GJGameLevel* level) {
        log::info("Level completed: {}", level->m_levelName);
    }
};

class $modify(EnhancedMenuLayer, MenuLayer) {
public:
    bool init() {
        if (!MenuLayer::init()) return false;
        
        if (Mod::get()->getSettingValue<bool>("show-toolkit-button")) {
            this->addToolkitButton();
        }
        
        return true;
    }

    void addToolkitButton() {
        auto toolkitBtn = CCMenuItemSpriteExtra::create(
            cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png"),
            this,
            menu_selector(EnhancedMenuLayer::onToolkitButton)
        );
        
        if (toolkitBtn) {
            toolkitBtn->setPosition({-200, 100});
            if (auto menu = this->getChildByID("bottom-menu")) {
                static_cast<cocos2d::CCMenu*>(menu)->addChild(toolkitBtn);
            }
        }
    }

    void onToolkitButton(cocos2d::CCObject* sender) {
        FLAlertLayer::create("Toolkits", "Toolkit features are configured in the mod settings!", "OK")->show();
    }
};

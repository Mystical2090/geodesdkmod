#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GameManager.hpp>

class $modify(MyPlayerObject, PlayerObject) {
    bool init(int p0, int p1, GJBaseGameLayer* p2, cocos2d::CCLayer* p3, bool p4) {
        if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;
        
        this->applyMiniMeEffects();
        
        return true;
    }
    
    void update(float dt) {
        PlayerObject::update(dt);

        this->applyMiniMeEffects();
        
        float scale = Mod::get()->getSettingValue<double>("player-scale");
        if (scale < 1.0f && Mod::get()->getSettingValue<bool>("enable-speed-boost")) {
            float speedMultiplier = Mod::get()->getSettingValue<double>("speed-multiplier");
            this->m_xVelocity *= speedMultiplier;
        }
        
        if (scale <= 0.3f && Mod::get()->getSettingValue<bool>("enable-mini-trail")) {
            this->addMiniTrail();
        }
    }
    
    void applyMiniMeEffects() {
        float scale = Mod::get()->getSettingValue<double>("player-scale");
        this->setScale(scale);
        
        float jumpBoost = Mod::get()->getSettingValue<double>("jump-boost");
        if (scale < 1.0f && Mod::get()->getSettingValue<bool>("enable-jump-boost")) {
            this->m_yVelocity *= jumpBoost;
        }
        
        if (Mod::get()->getSettingValue<bool>("enable-transparency")) {
            float opacity = Mod::get()->getSettingValue<double>("player-opacity");
            this->setOpacity(opacity * 255);
        }
        
        if (Mod::get()->getSettingValue<bool>("enable-spin-mode") && scale < 0.5f) {
            float spinSpeed = Mod::get()->getSettingValue<double>("spin-speed");
            this->setRotation(this->getRotation() + spinSpeed);
        }
    }
    
    void addMiniTrail() {
        if (rand() % 10 == 0) {
            auto particle = CCParticleSystemQuad::create();
            if (particle) {
                particle->setTexture(CCTextureCache::sharedTextureCache()->addImage("square02_001.png"));
                particle->setPosition(this->getPosition());
                particle->setScale(0.1f);
                particle->setLife(0.5f);
                particle->setStartColor({1.0f, 1.0f, 0.0f, 1.0f}); // Yellow
                particle->setEndColor({1.0f, 0.0f, 0.0f, 0.0f}); // Fade to transparent
                particle->setTotalParticles(1);
                
                if (this->getParent()) {
                    this->getParent()->addChild(particle);
                    particle->runAction(CCSequence::create(
                        CCDelayTime::create(0.5f),
                        CCRemoveSelf::create(),
                        nullptr
                    ));
                }
            }
        }
    }
    
    void playerDestroyed(bool p0) {
        PlayerObject::playerDestroyed(p0);
        
        float scale = Mod::get()->getSettingValue<double>("player-scale");
        if (scale <= 0.5f && Mod::get()->getSettingValue<bool>("enable-mini-death-effect")) {
            this->createMiniDeathEffect();
        }
    }
    
    void createMiniDeathEffect() {
        for (int i = 0; i < 5; i++) {
            auto spark = CCSprite::createWithSpriteFrameName("d_effect_01_001.png");
            if (spark && this->getParent()) {
                spark->setPosition(this->getPosition());
                spark->setScale(0.3f);
                spark->setColor({255, 255, 0}); // Yellow sparks
                this->getParent()->addChild(spark);

                float angle = (rand() % 360) * M_PI / 180.0f;
                float distance = 50.0f + (rand() % 50);
                
                auto moveAction = CCMoveBy::create(0.3f, {
                    cos(angle) * distance,
                    sin(angle) * distance
                });
                
                spark->runAction(CCSequence::create(
                    CCSpawn::create(
                        moveAction,
                        CCFadeOut::create(0.3f),
                        CCScaleBy::create(0.3f, 0.1f),
                        nullptr
                    ),
                    CCRemoveSelf::create(),
                    nullptr
                ));
            }
        }
    }
};

class $modify(MyGameManager, GameManager) {
    void reloadAll(bool p0, bool p1, bool p2) {
        GameManager::reloadAll(p0, p1, p2);

        if (Mod::get()->getSettingValue<bool>("enable-debug-mode")) {
            log::info("Mini-Me mod effects reloaded!");
        }
    }
};

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

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
        if (scale <= 0.3f && Mod::get()->getSettingValue<bool>("enable-mini-trail")) {
            this->addMiniTrail();
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

    void playerDestroyed(bool p0) {
        PlayerObject::playerDestroyed(p0);

        float scale = Mod::get()->getSettingValue<double>("player-scale");
        if (scale <= 0.5f && Mod::get()->getSettingValue<bool>("enable-mini-death-effect")) {
            this->createMiniDeathEffect();
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
    }
};

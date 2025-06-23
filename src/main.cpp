#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/PlayerObject.hpp>

class $modify(MyPlayerObject, PlayerObject) {
    bool init(int p0, int p1, GJBaseGameLayer* p2, cocos2d::CCLayer* p3, bool p4) {
        if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;
        
        float scale = Mod::get()->getSettingValue<double>("player-scale");
        this->setScale(scale);
        
        return true;
    }
    
    void update(float dt) {
        PlayerObject::update(dt);
        
        float scale = Mod::get()->getSettingValue<double>("player-scale");
        this->setScale(scale);
        
        float jumpBoost = Mod::get()->getSettingValue<double>("jump-boost");
        if (scale < 1.0f) {
            this->m_yVelocity *= jumpBoost;
        }
    }
};

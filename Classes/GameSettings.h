#ifndef _GAMESETTINGS_H_
#define _GAMESETTINGS_H_

#include "cocos2d.h"

class GameSettings : public cocos2d::LayerColor {
public:
    CREATE_FUNC(GameSettings);
    static cocos2d::Scene* createScene();
    virtual bool init(); 
    void onBack(cocos2d::Ref* pSender);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
#endif /* CC_TARGET_PLATFORM */
};

#endif /* _GAMESETTINGS_H_ */
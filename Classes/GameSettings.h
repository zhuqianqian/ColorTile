#ifndef _GAMESETTINGS_H_
#define _GAMESETTINGS_H_

#include "cocos2d.h"

class GameSettings : public cocos2d::LayerColor {
public:
    CREATE_FUNC(GameSettings);
    static cocos2d::Scene* createScene();
    virtual bool init(); 
    void onBack(cocos2d::Ref* pSender);
    virtual bool onTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event);
    virtual void onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
#endif /* CC_TARGET_PLATFORM */
private:
    float addTitle(const char *title);
    void setOption(cocos2d::Sprite* option, int status);
    cocos2d::Sprite* addOption(const char *option, cocos2d::Point *loc, int status);

    cocos2d::Size _wndSize;
    float _scale;
    cocos2d::Sprite* _sound;
    cocos2d::Sprite* _tutorial;
    int _touched;
    int _soundOn, _tutorialOn;
};

#endif /* _GAMESETTINGS_H_ */
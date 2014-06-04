#include "GameSettings.h"
#include "GameHome.h"
#include "colortile.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* GameSettings::createScene()
{
    auto scene = Scene::create();
    auto layer = GameSettings::create();
    scene->addChild(layer);
    return scene;
}

bool GameSettings::init() {
    if (!LayerColor::initWithColor(Color4B(238, 224, 200, 255)))
    {
        return false;
    }
    Size size = Director::getInstance()->getVisibleSize();
    float scale = size.width / DESIGN_WIDTH;
    auto title = LabelTTF::create("Settings", "Arial", 36 * scale);
    title->setPosition(size.width / 2, size.height - 64 * scale);
    this->addChild(title);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    this->setKeypadEnabled(true);
#endif
    return true;
}

void GameSettings::onBack(Ref * pSender) {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
    auto homeScene = GameHome::createScene();
    auto reScene = TransitionMoveInT::create(1.0, homeScene);
    Director::getInstance()->replaceScene(reScene);
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void GameSettings::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event){
    if(keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE) {
        onBack(nullptr);
    }
}
#endif /* CC_TARGET_PLATFORM */
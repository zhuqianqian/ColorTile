#include "GameSettings.h"
#include "GameHome.h"
#include "colortile.h"
#include "SimpleAudioEngine.h"
#include "strres.h"

USING_NS_CC;

#define TAG_CHECKBOX0   (2990)
#define TAG_CHECKBOX1   (2991)
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
    auto ud = UserDefault::getInstance();
    Point loc(0, 0);
    auto sr = StrRes::getInstance();
    this->_wndSize = Director::getInstance()->getVisibleSize();
    this->_scale = this->_wndSize.width / DESIGN_WIDTH;
    loc.y = addTitle(sr->getString(RSTR::settings));
    loc.y -= 12;
    this->_soundOn = ud->getIntegerForKey(SP_SOUND, 1);
    this->_sound = addOption(sr->getString(RSTR::sound_on), &loc, this->_soundOn);
    this->_tutorialOn = ud->getIntegerForKey(SP_TUTORIAL_CLASSIC, 1);
    if (this->_tutorialOn) {
        this->_tutorialOn = ud->getIntegerForKey(SP_TUTORIAL_MONO, 1);
    }
    this->_tutorial = addOption(sr->getString(RSTR::tutorial_on), &loc, this->_tutorialOn);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(GameSettings::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(GameSettings::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    int touched = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    this->setKeypadEnabled(true);
#endif
    return true;
}

float GameSettings::addTitle(const char *title) {
    auto sTitle = Sprite::create();
    Rect r;
    r.setRect(0, 0, this->_wndSize.width, 64*this->_scale);
    sTitle->setTextureRect(r);
    sTitle->setAnchorPoint({ 0.0f, 1.0f });
    sTitle->setPosition(0, this->_wndSize.height);
    sTitle->setColor(Color3B::WHITE);
    auto labelTitle = LabelTTF::create(title, "Arial", 36 * this->_scale);
    labelTitle->setColor(Color3B::BLACK);
    labelTitle->setPosition(this->_wndSize.width / 2, 64 * this->_scale/2);
    auto backItem = MenuItemImage::create("back0.png", "back1.png", CC_CALLBACK_1(GameSettings::onBack, this));
    sTitle->addChild(labelTitle);
    backItem->setAnchorPoint({ 0.0f, 0.0f });
    backItem->setPosition(0, 0);
    auto menu = Menu::create(backItem, nullptr);
    menu->setPosition(Point::ZERO);
    sTitle->addChild(menu);
    this->addChild(sTitle);
    return (this->_wndSize.height - 64 * this->_scale);
}

Sprite* GameSettings::addOption(const char *option, Point *loc, int status) {
    char pngs[][16] = { "checkbox0.png", "checkbox1.png" };
    int tags[] = { TAG_CHECKBOX0, TAG_CHECKBOX1};
    auto bkg = Sprite::create();
    Rect r;
    r.setRect(0, 0, this->_wndSize.width, 48 * this->_scale);
    bkg->setTextureRect(r);
    bkg->setAnchorPoint({ 0.0f, 1.0f });
    bkg->setPosition(0, (*loc).y - 12 * this->_scale);
    bkg->setColor(Color3B(238, 224, 0));
    auto label = LabelTTF::create(option, "Arial", 28 * this->_scale, 
        Size::ZERO, TextHAlignment::LEFT, TextVAlignment::CENTER);
    label->setColor(Color3B::BLACK);
    label->setAnchorPoint({ 0.0f, 0.5f });
    label->setPosition(32 * this->_scale, 24 * this->_scale);
    bkg->addChild(label); 
    auto checkbox0 = Sprite::create(pngs[0]);
    checkbox0->setScale(this->_scale, this->_scale);
    checkbox0->setAnchorPoint({ 1.0f, 0.0f });
    checkbox0->setPosition(this->_wndSize.width - 32 * this->_scale, 0);
    bkg->addChild(checkbox0, 0, tags[0]);
    auto checkbox1 = Sprite::create(pngs[1]);
    checkbox1->setScale(this->_scale, this->_scale);
    checkbox1->setAnchorPoint({ 1.0f, 0.0f });
    checkbox1->setPosition(this->_wndSize.width - 32 * this->_scale, 0);
    bkg->addChild(checkbox1, 0, tags[1]);
    if (!status) {
        checkbox1->setVisible(false);
    }
    this->addChild(bkg);
    (*loc).y -= 60 * this->_scale;
    return bkg;
}

void GameSettings::setOption(Sprite * option, int status) {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
    int tags[] = { TAG_CHECKBOX0, TAG_CHECKBOX1 };
    auto checkbox = option->getChildByTag(tags[1]);
    if (status) {
        checkbox->setVisible(true);
    }
    else {
        checkbox->setVisible(false);
    }
    if (option == this->_sound) {
        auto ud = UserDefault::getInstance();
        ud->setIntegerForKey(SP_SOUND, status);
        ud->flush();
    }
    else if (option == this->_tutorial) {
        auto ud = UserDefault::getInstance();
        ud->setIntegerForKey(SP_TUTORIAL_CLASSIC, status);
        ud->setIntegerForKey(SP_TUTORIAL_MONO, status);
        ud->flush();
    }
}

void GameSettings::onBack(Ref * pSender) {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
    auto homeScene = GameHome::createScene();
    auto reScene = TransitionMoveInT::create(0.4f, homeScene);
    Director::getInstance()->replaceScene(reScene);
}

bool GameSettings::onTouchBegan(Touch * touch, Event * event) {
    Point touchLoc = this->convertTouchToNodeSpace(touch);
    if (this->_sound->getBoundingBox().containsPoint(touchLoc) ||
        this->_tutorial->getBoundingBox().containsPoint(touchLoc)) {
        this->_touched = 1;
    }
    return true;
}

void GameSettings::onTouchEnded(Touch * touch, Event * event) {
    Point touchLoc = this->convertTouchToNodeSpace(touch);
    if (this->_touched) {
        if (this->_sound->getBoundingBox().containsPoint(touchLoc)) {
            this->_soundOn ^= 0x01;
            setOption(this->_sound, this->_soundOn);
            CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(this->_soundOn);
        }
        else if (this->_tutorial->getBoundingBox().containsPoint(touchLoc)) {
            this->_tutorialOn ^= 0x01;
            setOption(this->_tutorial, this->_tutorialOn);
        }
    }
    this->_touched = 0;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void GameSettings::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event){
    if(keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE) {
        onBack(nullptr);
    }
}
#endif /* CC_TARGET_PLATFORM */
#include "GameSettings.h"
#include "GameHome.h"
#include "colortile.h"
#include "SimpleAudioEngine.h"
#include "strres.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "gpghelper.h"
#endif /* CC_TARGET_PLATFORM */

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
    if (!LayerColor::initWithColor(COLOR4_BKG))
    {
        return false;
    }
    
    auto ud = UserDefault::getInstance();
    Point loc(0, 0);
    auto sr = StrRes::getInstance();
    this->_size = Director::getInstance()->getVisibleSize();
    this->_xScale = this->_size.width / DESIGN_WIDTH;
    this->_tileSize = 80 * this->_xScale;
    this->_halfSize = this->_tileSize / 2;
    float margin = 18 * this->_xScale;
    loc.y = addTitle(sr->getString(RSTR::app_name));
    loc.y -= margin;
    loc.y -= addHeader(sr->getString(RSTR::settings), &loc);
    loc.y -= margin;
    this->_soundOn = ud->getIntegerForKey(SP_SOUND, 1);
    this->_sound = addOption(sr->getString(RSTR::sound_on), &loc, this->_soundOn);
    this->_tutorialOn = ud->getIntegerForKey(SP_TUTORIAL_CLASSIC, 1);
    if (this->_tutorialOn) {
        this->_tutorialOn = ud->getIntegerForKey(SP_TUTORIAL_MONO, 1);
    }
    this->_tutorial = addOption(sr->getString(RSTR::tutorial_on), &loc, this->_tutorialOn);
    loc.y -= margin * 2;
    loc.y -= addHeader(sr->getString(RSTR::about), &loc);
    loc.y -= margin;
    addMenus(&loc);
    auto labelVersion = LabelTTF::create(sr->getString(RSTR::version), "Arial", 16 * this->_xScale);
    labelVersion->setColor(Color3B(64,64,64));
    labelVersion->setAnchorPoint({ 0.5f, 0.0f });
    labelVersion->setPosition(this->_size.width / 2, margin);
    this->addChild(labelVersion, 1);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(GameSettings::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(GameSettings::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    int touched = 0;
    initRandomBkg();
    this->schedule(schedule_selector(GameSettings::randomTile), 1.0, kRepeatForever, 0.5);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    this->setKeypadEnabled(true);
#endif
    return true;
}

float GameSettings::addTitle(const char *title) {
    auto sTitle = Sprite::create();
    Rect r;
    r.setRect(0, 0, this->_size.width, 72*this->_xScale);
    sTitle->setTextureRect(r);
    sTitle->setAnchorPoint({ 0.0f, 1.0f });
    sTitle->setPosition(0, this->_size.height);
    sTitle->setColor(Color3B::WHITE);
    auto labelTitle = LabelTTF::create(title, "Arial", 40 * this->_xScale);
    labelTitle->setColor(Color3B::BLACK);
    labelTitle->setPosition(this->_size.width / 2, 36 * this->_xScale);
    auto backItem = MenuItemImage::create("back0.png", "back1.png", CC_CALLBACK_1(GameSettings::onBack, this));
    sTitle->addChild(labelTitle);
    backItem->setAnchorPoint({ 0.0f, 0.0f });
    backItem->setPosition(0, 0);
    backItem->setScale(this->_xScale * 72 / 64.0f);
    auto menu = Menu::create(backItem, nullptr);
    menu->setPosition(Point::ZERO);
    sTitle->addChild(menu);
    this->addChild(sTitle, 1);
    return (this->_size.height - 72 * this->_xScale);
}

float GameSettings::addHeader(const char *header, Point * loc) {
    Rect r;
    float height;
    static float margin = 16 * this->_xScale,
        length = this->_size.width - margin - margin - margin;
    Sprite * line = Sprite::create();
    r.setRect(0, 0, length, 2.0 * this->_xScale);
    line->setTextureRect(r);
    line->setColor(Color3B::BLACK);
    auto label = LabelTTF::create(header, "Arial", 32 * this->_xScale,
        Size::ZERO, TextHAlignment::LEFT);
    label->setColor(Color3B::BLACK);
    label->setAnchorPoint({ 0.0f, 1.0f });
    label->setPosition(margin, loc->y);
    this->addChild(label, 1);
    height = label->getContentSize().height;
    line->setAnchorPoint({ 0.0f, 1.0f });
    line->setPosition(margin, loc->y - height);
    this->addChild(line, 1);
    return height;
}

float GameSettings::addMenus(Point * loc) {
    int width = 180 * this->_xScale,
        height = 48 * this->_xScale,
        fntSize = 28 * this->_xScale,
        border = 2 * this->_xScale,
        margin = 32 * this->_xScale;
    auto sr = StrRes::getInstance();
    auto misMore = GameHome::createTextButton(sr->getString(RSTR::more), width, height, fntSize, border,
        COLOR3_BKG, Color3B::GRAY, CC_CALLBACK_1(GameSettings::onMoreGames, this), Color3B::BLACK);
    auto misRate = GameHome::createTextButton(sr->getString(RSTR::rate), width, height, fntSize, border,
        COLOR3_BKG, Color3B::GRAY, CC_CALLBACK_1(GameSettings::onMoreGames, this), Color3B::BLACK);
    misMore->setAnchorPoint({ 0.0f, 1.0f });
    misRate->setAnchorPoint({ 1.0f, 1.0f });
    misMore->setPosition(margin, loc->y - 12 * this->_xScale);
    misRate->setPosition(this->_size.width - margin, loc->y - 12 * this->_xScale);
    auto menu = Menu::create(misMore, misRate, nullptr);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);
    loc->y -= 60 * this->_xScale;
    return 0;
}

Sprite* GameSettings::addOption(const char *option, Point *loc, int status) {
    static float margin = 32 * this->_xScale,
        length = this->_size.width - margin - margin;
    char pngs[][16] = { "checkbox0.png", "checkbox1.png" };
    int tags[] = { TAG_CHECKBOX0, TAG_CHECKBOX1};
    auto bkg = Sprite::create();
    Rect r;
    r.setRect(0, 0, length, 48 * this->_xScale);
    bkg->setTextureRect(r);
    bkg->setAnchorPoint({ 0.0f, 1.0f });
    bkg->setPosition(margin, (*loc).y - 12 * this->_xScale);
    bkg->setOpacity(0);
    auto label = LabelTTF::create(option, "Arial", 28 * this->_xScale, 
        Size::ZERO, TextHAlignment::LEFT, TextVAlignment::CENTER);
    label->setColor(Color3B::BLACK);
    label->setAnchorPoint({ 0.0f, 0.5f });
    label->setPosition(0, 24 * this->_xScale);
    bkg->addChild(label); 
    auto checkbox0 = Sprite::create(pngs[0]);
    checkbox0->setScale(this->_xScale, this->_xScale);
    checkbox0->setAnchorPoint({ 1.0f, 0.0f });
    checkbox0->setPosition(length , 0);
    bkg->addChild(checkbox0, 0, tags[0]);
    auto checkbox1 = Sprite::create(pngs[1]);
    checkbox1->setScale(this->_xScale, this->_xScale);
    checkbox1->setAnchorPoint({ 1.0f, 0.0f });
    checkbox1->setPosition(length , 0);
    bkg->addChild(checkbox1, 0, tags[1]);
    if (!status) {
        checkbox1->setVisible(false);
    }
    this->addChild(bkg, 1);
    (*loc).y -= 60 * this->_xScale;
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

void GameSettings::onMoreGames(Ref * pSender) {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    doRate();
#endif /* CC_TARGET_PLATFORM */
}

void GameSettings::onRateApp(Ref * pSender) {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    doMoreGames();
#endif /* CC_TARGET_PLATFORM */
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
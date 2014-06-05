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
    float margin = 18 * this->_scale;
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
    loc.x = this->_wndSize.width;
    loc.y -= margin;
    addLabel(sr->getString(RSTR::version), &loc, 0);
    loc.x /= 2;
    loc.y -= 80 * this->_scale;
    addLabel(sr->getString(RSTR::more), &loc, 1);
    loc.x = this->_wndSize.width * 3 / 2;
    addLabel(sr->getString(RSTR::rate), &loc, 1);
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
    r.setRect(0, 0, this->_wndSize.width, 72*this->_scale);
    sTitle->setTextureRect(r);
    sTitle->setAnchorPoint({ 0.0f, 1.0f });
    sTitle->setPosition(0, this->_wndSize.height);
    sTitle->setColor(Color3B::WHITE);
    auto labelTitle = LabelTTF::create(title, "Arial", 40 * this->_scale);
    labelTitle->setColor(Color3B::BLACK);
    labelTitle->setPosition(this->_wndSize.width / 2, 36 * this->_scale);
    auto backItem = MenuItemImage::create("back0.png", "back1.png", CC_CALLBACK_1(GameSettings::onBack, this));
    sTitle->addChild(labelTitle);
    backItem->setAnchorPoint({ 0.0f, 0.0f });
    backItem->setPosition(0, 0);
    backItem->setScale(72 / 64.0f);
    auto menu = Menu::create(backItem, nullptr);
    menu->setPosition(Point::ZERO);
    sTitle->addChild(menu);
    this->addChild(sTitle);
    return (this->_wndSize.height - 72 * this->_scale);
}

float GameSettings::addHeader(const char *header, Point * loc) {
    Rect r;
    float height;
    static float margin = 16 * this->_scale,
        length = this->_wndSize.width - margin - margin - margin;
    Sprite * line = Sprite::create();
    r.setRect(0, 0, length, 2.0 * this->_scale);
    line->setTextureRect(r);
    line->setColor(Color3B::BLACK);
    auto label = LabelTTF::create(header, "Arial", 32 * this->_scale,
        Size::ZERO, TextHAlignment::LEFT);
    label->setColor(Color3B::BLACK);
    label->setAnchorPoint({ 0.0f, 1.0f });
    label->setPosition(margin, loc->y);
    this->addChild(label);
    height = label->getContentSize().height;
    line->setAnchorPoint({ 0.0f, 1.0f });
    line->setPosition(margin, loc->y - height);
    this->addChild(line);
    return height;
}

Node* GameSettings::addLabel(const char * text, Point * loc, int clickable) {
    static float border = 2 * this->_scale;
    LabelTTF * label = LabelTTF::create(text, "Arial", 28 * this->_scale);
    label->setColor(Color3B::BLACK);
    label->setAnchorPoint({ 0.5f, 1.0f });
    label->setPosition(loc->x / 2, loc->y);
    this->addChild(label);
    if (!clickable) {
        return label;
    }
    Rect r;
    r.setRect(0, 0, label->getContentSize().width, border);
    auto s = Sprite::create();
    s->setTextureRect(r);
    s->setColor(Color3B::BLACK);
    s->setAnchorPoint({ 0.5f, 1.0f });
    s->setPosition(loc->x / 2, loc->y - label->getContentSize().height);
    this->addChild(s);
    return label;
}

Sprite* GameSettings::addOption(const char *option, Point *loc, int status) {
    static float margin = 32 * this->_scale,
        length = this->_wndSize.width - margin - margin;
    char pngs[][16] = { "checkbox0.png", "checkbox1.png" };
    int tags[] = { TAG_CHECKBOX0, TAG_CHECKBOX1};
    auto bkg = Sprite::create();
    Rect r;
    r.setRect(0, 0, length, 48 * this->_scale);
    bkg->setTextureRect(r);
    bkg->setAnchorPoint({ 0.0f, 1.0f });
    bkg->setPosition(margin, (*loc).y - 12 * this->_scale);
    bkg->setOpacity(0);
    auto label = LabelTTF::create(option, "Arial", 28 * this->_scale, 
        Size::ZERO, TextHAlignment::LEFT, TextVAlignment::CENTER);
    label->setColor(Color3B::BLACK);
    label->setAnchorPoint({ 0.0f, 0.5f });
    label->setPosition(0, 24 * this->_scale);
    bkg->addChild(label); 
    auto checkbox0 = Sprite::create(pngs[0]);
    checkbox0->setScale(this->_scale, this->_scale);
    checkbox0->setAnchorPoint({ 1.0f, 0.0f });
    checkbox0->setPosition(length , 0);
    bkg->addChild(checkbox0, 0, tags[0]);
    auto checkbox1 = Sprite::create(pngs[1]);
    checkbox1->setScale(this->_scale, this->_scale);
    checkbox1->setAnchorPoint({ 1.0f, 0.0f });
    checkbox1->setPosition(length , 0);
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
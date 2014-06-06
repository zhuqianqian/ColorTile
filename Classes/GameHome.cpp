#define NDEBUG
#include "SimpleAudioEngine.h"
#include "colortile.h"
#include "GameHome.h"
#include "GameNormal.h"
#include "GameMono.h"
#include "Dialog.h"
#include "GameSettings.h"
#include "strres.h"

#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
#include "gpghelper.h"

static int __asked = 0;
#endif /* CC_TARGET_PLATFORM */

USING_NS_CC;
using namespace z299studio;

#define CT_CREATE_TILE(var, i, r, c)  \
	(var) = Sprite::create(); \
	(var)->setColor((c)); \
	(var)->setTextureRect((r)); \
	(var)->setOpacity(0); \
	this->addChild((var)); \
	this->_sprites[(i)] = (var); \
	this->_sindex[(i)] = 1; 

Scene* GameHome::createScene()
{
    auto scene = Scene::create();
	auto layer = GameHome::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool GameHome::init()
{
    if (!LayerColor::initWithColor(COLOR4_BKG))
    {
        return false;
    }
	
	float btnWidth, btnHeight, btnBorder, btnFont;
	Color3B color0(246, 94, 59);
	Color3B color1(255, 64, 128);
	StrRes * sr = StrRes::getInstance();
	srand(time(NULL));
    this->_size = Director::getInstance()->getVisibleSize();
	this->_xScale = this->_size.width / DESIGN_WIDTH;
	btnWidth = 280 * this->_xScale;
	btnHeight = 96 * this->_xScale;
	btnBorder = 2 * this->_xScale;
	btnFont = 36 * this->_xScale;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	this->setKeypadEnabled(true);
#endif
	Menu *menu;
	MenuItemSprite * lbItem = NULL;
	this->useGoogleGame = UserDefault::getInstance()->getBoolForKey(SP_ENABLE_GOOGLE, false);
    auto settingItem = MenuItemImage::create("settings0.png", "settings1.png", CC_CALLBACK_1(GameHome::onSettings, this));
    settingItem->setScale(this->_xScale);
    settingItem->setAnchorPoint({ 1.0f, 0.0f });
    settingItem->setPosition(this->_size.width - 32 * this->_xScale, 32 * this->_xScale);
	auto newItem = createTextButton(sr->getString(RSTR::new_game), btnWidth, btnHeight, btnFont, btnBorder, 
        Color3B(78, 205, 168), Color3B(157, 230, 208), CC_CALLBACK_1(GameHome::onNewGame, this, GAME_NORMAL), COLOR3_BKG);
	auto monoItem = createTextButton(sr->getString(RSTR::monocolor), btnWidth, btnHeight, btnFont, btnBorder,
        Color3B(73, 217, 104), Color3B(157, 236, 175), CC_CALLBACK_1(GameHome::onNewGame, this, GAME_MONOCLR), COLOR3_BKG);
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
	lbItem = createTextButton(sr->getString(RSTR::leaderboard), btnWidth, btnHeight, btnFont, btnBorder,
        Color3B(160, 231, 71), Color3B(193, 241, 141), CC_CALLBACK_1(GameHome::onLeaderboard, this), COLOR3_BKG);
	if (!this->useGoogleGame && !__asked && UserDefault::getInstance()->getIntegerForKey(SP_ASK_GOOGLE, 1)) {
		this->schedule(schedule_selector(GameHome::askUseGooglePlay), 0.1f, 0, 0.5f);
		__asked = 1;
	}
#endif /* CC_TARGET_PLATFORM */
	if (GAME_OVER != UserDefault::getInstance()->getIntegerForKey(SP_GAME_STATUS, GAME_OVER)) {
		auto resumeItem = createTextButton(sr->getString(RSTR::resume_game), btnWidth, btnHeight, btnFont, btnBorder,
            Color3B(78, 172, 197), Color3B(175, 217, 218), CC_CALLBACK_1(GameHome::onNewGame, this, GAME_RESUME), COLOR3_BKG);
		menu = Menu::create(resumeItem, newItem, monoItem, lbItem, NULL);
	}
	else {
		menu = Menu::create(newItem, monoItem, lbItem, NULL);
	}
	this->_tileSize = 80 * this->_xScale;
	this->_halfSize = this->_tileSize / 2;
	menu->alignItemsVerticallyWithPadding(32 * this->_xScale);
	menu->setPosition(this->_size.width / 2, this->_size.height / 2);
    auto menuSettings = Menu::create(settingItem, nullptr);
    menuSettings->setPosition(Point::ZERO);
    this->addChild(menuSettings, 1);
	this->addChild(menu, 1);
	this->initRandomBkg();
	this->schedule(schedule_selector(GameHome::randomTile), 1.0, kRepeatForever, 0.5);
	this->_menu = menu;
    return true;
}

MenuItemSprite* GameHome::createTextButton(const char *text,
    int width, int height,
    int fntSize, int border,
	const cocos2d::Color3B normal,
	const cocos2d::Color3B pressed,
	const cocos2d::ccMenuCallback &callback,
    const Color3B textColor)
{
	Rect r1, r2; 
	r1.setRect(0, 0, width, height);
	r2.setRect(border, border, width - 2 * border, height - 2 * border);
	auto bkgNormal = Sprite::create();
	auto bkgPressed = Sprite::create();
	auto rectNormal = Sprite::create();
	auto rectPressed = Sprite::create();
	auto label = LabelTTF::create(text, "Arial", fntSize);
    label->setColor(textColor);
    bkgNormal->setColor(textColor);
    bkgPressed->setColor(textColor);
	bkgNormal->setTextureRect(r1);
	bkgPressed->setTextureRect(r1);
	rectNormal->setColor(normal);
	rectNormal->setTextureRect(r2);
	rectPressed->setColor(pressed);
	rectPressed->setTextureRect(r2);
	label->setPosition(width / 2, height / 2);
	rectNormal->setPosition(width / 2, height / 2);
	rectPressed->setPosition(width / 2, height / 2);
	bkgNormal->addChild(rectNormal);
	bkgPressed->addChild(rectPressed);
	auto mis = MenuItemSprite::create(bkgNormal, bkgPressed, callback);
    mis->addChild(label);
    return mis;
}

void GameHome::initRandomBkg(int max) {
	this->_max = max;
	if (max > 8) {
		this->_max = 8;
	}
	Sprite *s;
	Rect rect;
	rect.setRect(0, 0, this->_tileSize, this->_tileSize);
	CT_CREATE_TILE(s, 0, rect, COLOR_0);
	CT_CREATE_TILE(s, 1, rect, COLOR_0);
	CT_CREATE_TILE(s, 2, rect, COLOR_1);
	CT_CREATE_TILE(s, 3, rect, COLOR_1);
	CT_CREATE_TILE(s, 4, rect, COLOR_2);
	CT_CREATE_TILE(s, 5, rect, COLOR_2);
	CT_CREATE_TILE(s, 6, rect, COLOR_3);
	CT_CREATE_TILE(s, 7, rect, COLOR_3);
}

void GameHome::randomTile(float dt) {
	
	Sprite * sprite;
	int n = rand() % this->_max;
	if (this->_sindex[n]) {
		sprite = this->_sprites[n];
		float duration = (0x01ff & rand() + 0x007f) / 0x80;
		int x = this->_halfSize + rand() % (int)(this->_size.width- this->_tileSize);
		int y = this->_halfSize + rand() % (int)(this->_size.height - this->_tileSize);
		sprite->setPosition(x, y);
		sprite->setOpacity(0);
		auto fadeIn = FadeIn::create(duration);
		auto fadeOut = FadeOut::create(duration);
		auto finish = CCCallFuncND::create(this, callfuncND_selector(GameHome::onFadeFinish), (void *)n);
		this->_sindex[n] = 0;
		auto seq = Sequence::create(fadeIn, fadeOut, finish, NULL);
		sprite->runAction(seq);
	}
}

void GameHome::onFadeFinish(Node * node, void * index) {
	int n = (int)index;
	this->_sindex[n] = 1;
}

void GameHome::onSettings(Ref * pSender) {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
    auto scene = GameSettings::createScene();
    auto reScene = TransitionMoveInB::create(0.4f, scene);
    Director::getInstance()->replaceScene(reScene);
}

void GameHome::onNewGame(Ref* pSender, int mode)
{
	int gm;
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
	if (mode == GAME_RESUME) {
		GameBase::gameStartNew = 0;
		gm = UserDefault::getInstance()->getIntegerForKey(SP_GAME_MODE, GAME_NORMAL);
	}
	else {
		if (GAME_OVER != UserDefault::getInstance()->getIntegerForKey(SP_GAME_STATUS, GAME_OVER)) {
            auto sr = StrRes::getInstance();
            Dialog::build()->setContentScale(this->_xScale)
                ->show(nullptr, sr->getString(RSTR::overwrite_game),
                CC_CALLBACK_1(GameHome::confirmCallback, this, mode),
                sr->getString(RSTR::yes), sr->getString(RSTR::no));
			return;
		}
		GameBase::gameStartNew = 1;
		gm = mode;
	}
	Scene * mainScene;
	if (gm == GAME_NORMAL) {
	    mainScene = GameNormal::createScene();
    }
	else {
		mainScene = GameMono::createScene();
	}
	auto reScene = TransitionFadeTR::create(1.0, mainScene);
	Director::getInstance()->replaceScene(reScene);
}


bool GameHome::confirmCallback(int answer, int mode) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
	if (answer) {
		GameBase::gameStartNew = 1;
		Scene * mainScene;
		if (mode == GAME_NORMAL) {
			mainScene = GameNormal::createScene();
		}
		else {
			mainScene = GameMono::createScene();
		}
		auto reScene = TransitionFadeTR::create(1.0, mainScene);
		Director::getInstance()->replaceScene(reScene);
	}
    return true;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void GameHome::onLeaderboard(Ref * pSender) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN); 
	if (this->useGoogleGame && ::isSignedIn()) {
		::showAllLeaderboards();		
	}
	else {
		::beginUserInitiatedSigned();
		this->useGoogleGame = true;
	}
}

void GameHome::askUseGooglePlay(float dt) {
    auto sr = StrRes::getInstance();
    Dialog::build()->setContentScale(this->_xScale)
        ->show(nullptr, sr->getString(RSTR::use_google_ask),        
        CC_CALLBACK_1(GameHome::respondGooglePlay, this),
        sr->getString(RSTR::signin), sr->getString(RSTR::no));
}

bool GameHome::respondGooglePlay(int answer) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
	if (answer) {
		::beginUserInitiatedSigned();
		//UserDefault::getInstance()->setBoolForKey(SP_ENABLE_GOOGLE, true);
	}
	else {
		UserDefault::getInstance()->setIntegerForKey(SP_ASK_GOOGLE, 0);
	}
    return true;
}

#endif /* CC_TARGET_PLATFORM */

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void GameHome::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE) {
        if(Dialog::getInstance()) {
            Dialog::getInstance()->dismiss();
            return;
        }
		Director::getInstance()->end();
		StrRes::killInstance();
	}
}
#endif /* CC_TARGET_PLATFORM */

#include "GameLoad.h"
#include "GameHome.h"
#include "colortile.h"
#include "SimpleAudioEngine.h"

#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
#include "gpghelper.h"
#endif /* CC_TARGET_PLATFORM */

USING_NS_CC;

Scene* GameLoad::createScene()
{
	auto scene = Scene::create();
	auto layer = GameLoad::create();
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool GameLoad::init()
{
	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 255)))
	{
		return false;
	}
	//this->_tried = 3;
	Size size = Director::getInstance()->getVisibleSize();
	float scale = size.width / DESIGN_WIDTH;
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(
        UserDefault::getInstance()->getIntegerForKey(SP_SOUND, 1));
	auto bkg = Sprite::create("load_screen.png");
	bkg->setScale(scale, scale);
	bkg->setPosition(size.width / 2, size.height / 2);
	this->addChild(bkg);
	MenuItemFont::setFontName("Arial");
	MenuItemFont::setFontSize(32 * scale);
	this->schedule(schedule_selector(GameLoad::onLoadFinish), 0.1f, 0, 1.2f);
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
	//auto ud = UserDefault::getInstance();
	//this->_enableGoogle = ud->getBoolForKey(SP_ENABLE_GOOGLE, false);
#endif /* CC_TARGET_PLATFORM */
	return true;
}

void GameLoad::onLoadFinish(float dt) {
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
	//if (this->_enableGoogle && this->_tried) {
	//	if (!::isSignedIn()) {
	//		this->unschedule(schedule_selector(GameLoad::onLoadFinish));
	//		this->schedule(schedule_selector(GameLoad::onLoadFinish), 0.1f, 0, 0.2f);
	//		this->_tried--;
	//		return;
	//	}
	//}
#endif /* CC_TARGET_PLATFORM */
	auto home = GameHome::createScene();
	auto reScene = TransitionMoveInR::create(0.25f, home);
	Director::getInstance()->replaceScene(reScene);
}
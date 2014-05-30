#include "AppDelegate.h"
#include "colortile.h"
#include "GameBase.h"
#include "GameLoad.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
	Size frameSize;
    director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("Color Tile");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		glview->setFrameSize(320,480);
#endif
        director->setOpenGLView(glview);
    }
	
	frameSize = glview->getFrameSize();
    director->setAnimationInterval(1.0 / 60);
    auto scene = GameLoad::createScene();
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	director->pause();
 //   director->stopAnimation();
	GameBase *game;
	auto scene = director->getRunningScene();
	if (game = dynamic_cast<GameBase *>(scene->getChildByTag(TAG_GAMESCENE))) {
		game->onSaveProgress();
	}
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	director->resume();
	//director->startAnimation();	
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

#ifndef __GAMEHOME_H__
#define __GAMEHOME_H__

#include "cocos2d.h"

class GameHome : public cocos2d::LayerColor
{
public:
	cocos2d::MenuItemSprite * createTextButton(const char *text,
		float width, float height, 
		float fntSize, float border,
		const cocos2d::Color3B normal,
		const cocos2d::Color3B pressed,
		const cocos2d::ccMenuCallback &callback);
	static cocos2d::MenuItemSprite * createDialogButton(const char *text,
		float width, float height,
		float fntSize, float border,
		const cocos2d::Color3B normal,
		const cocos2d::Color3B pressed,
		const int style,
		const cocos2d::ccMenuCallback &callback);
    static cocos2d::Scene* createScene();
	virtual bool init();  
    void onNewGame(cocos2d::Ref* pSender, int mode);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	void askUseGooglePlay(float dt);
	void respondGooglePlay(cocos2d::Ref* pSender, int answer);
	void onLeaderboard(cocos2d::Ref *pSender);
#endif /* CC_TARGET_PLATFORM */
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
#endif /* CC_TARGET_PLATFORM */
    CREATE_FUNC(GameHome);

private:

	void randomTile(float dt);
	void initRandomBkg(int max = 8);
	void onFadeFinish(cocos2d::Node *, void * index);

	void confirmOverwrite(int mode);
	void confirmCallback(cocos2d::Ref * pSender, int answer, int mode);
	cocos2d::Size _size;
	cocos2d::Sprite *_sprites[8];
	float _xScale; 
	float _tileSize;
	float _halfSize;
	int _sindex[8];
	int _max;
	bool useGoogleGame;
	cocos2d::Menu * _menu;
};

#endif // __GAMEHOME_H__

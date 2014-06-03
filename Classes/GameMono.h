#ifndef _GAMEMONO_H_
#define _GAMEMONO_H_

#include "GameBase.h"

class GameMono : public GameBase {
public:
	CREATE_FUNC(GameMono);
	static cocos2d::Scene * createScene();
	virtual bool init();
	int updateScore(int n) override;
	virtual void startTutorial() override;
	void formatScore(char *str, int score) override;
	void onGiveUp(cocos2d::Ref* pSender) override;
	bool dialogCallback(int answer);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
#endif
protected:
	int checkGameOver();
	bool saveBest(char *str);
};

#endif /* _GAMEMONO_H_*/
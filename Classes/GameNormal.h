#ifndef _GAMENORMAL_H_
#define _GAMENORMAL_H_

#include "GameBase.h"

class GameNormal :public GameBase {
public:
	CREATE_FUNC(GameNormal);
	static cocos2d::Scene * createScene();
	virtual bool init();
	int updateScore(int n) override;
	virtual void startTutorial() override;

protected:
	int checkGameOver();
	bool saveBest(char *str);
};
#endif /* _GAMENORMAL_H_ */
#ifndef _GAMELOAD_H_
#define _GAMELOAD_H_

#include "cocos2d.h"
class GameLoad : public cocos2d::LayerColor {
public:
	CREATE_FUNC(GameLoad);
	static cocos2d::Scene * createScene();
	virtual bool init();

	void onLoadFinish(float dt);

};

#endif /* _GAMELOAD_H_ */
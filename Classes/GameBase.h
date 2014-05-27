#ifndef _GAMEBASE_H_
#define _GAMEBASE_H_

#include "colortile.h"

//typedef std::function<void()> callback;

class GameBase : public cocos2d::LayerColor {
public:
	static int gameStartNew;

	CREATE_FUNC(GameBase);
	~GameBase() {
		if (this->_tileColors != nullptr) {
			delete this->_tileColors;
			delete this->_tileSprites;
			delete this->_swipeRecords;
			delete this->_stackData;
			this->_tileColors = nullptr;
			this->_tileSprites = nullptr;
			this->_swipeRecords = nullptr;
			this->_stackData = nullptr;
		}
	}
	virtual bool init();
	void onBack(cocos2d::Ref * pSender);
	void onShare(cocos2d::Ref* pSender);
	virtual void restartGame(cocos2d::Ref* pSender);
	void onUndo(cocos2d::Ref* pSender);
	virtual void onGiveUp(cocos2d::Ref* pSender);

	virtual bool onTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event);
	virtual void onTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event);
	virtual void onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event);
	virtual void onSaveProgress();
	virtual void onLoadProgress(int nColors = 4);
	virtual void startTutorial();
	virtual void beginAutoSwipe(float dt);
	virtual void tutorialNext();
	virtual void tutorialComplete();
	virtual void tutorialCleanup();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
#endif

protected:
	bool initGame();
	void setHeader();
	bool setTiles(int x, int y, int colors = 4, char * tile = nullptr);
	void onGameOver();
	void removeGameOverDialog();	
	virtual void setCandColors(int n, int init = 0, char *data = nullptr);
	virtual void onTileUpdate();
	virtual void reset();
	virtual int checkGameOver();
	virtual bool saveBest(char * str);
	virtual int updateScore(int n);
	virtual void formatScore(char *str, int score);

	virtual void pushStack(int n);
	virtual void popStack();


	int _xCount, _yCount, _nColors;
	int _status;
	char* _tileColors;
	int* _swipeRecords;
	int _recIndex;
	cocos2d::Sprite ** _tileSprites;
	float _xScale, _dpScale;
	float _xBoard, _yBoard;
	float _headerHeight;
	int _score, _best;
	char _scoreStr[8];
	int _tileSize, _tileInterval;
	int _regFlag;

	cocos2d::Sprite* _candidates[8];
	char _candColor[8];
	char *_strScore, *_strBest;
	cocos2d::LabelAtlas* _laScore, * _laBest;
	cocos2d::Size _wndSize;
	cocos2d::Sprite* _gameOverBkg;
	cocos2d::UserDefault * _ud;

	int _mode;
	bool _useGoogleGame;

	char * _stackData;
	char * _stackPtrs[8];
	int _stackPointer;
	int _stackCap;
	int _maxStackCap;

	cocos2d::MenuItem * _undo, *_giveup;
	cocos2d::LabelTTF * _maxStackLabel;
	cocos2d::Menu * _funcbar;
};

#endif /* _GAMEBASE_H_ */
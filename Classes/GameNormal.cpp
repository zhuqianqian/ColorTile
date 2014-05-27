#include "GameNormal.h"
#include "strres.h"

USING_NS_CC;

Scene* GameNormal::createScene()
{
	auto scene = Scene::create();
	auto layer = GameNormal::create();
	scene->addChild(layer, 0, TAG_GAMESCENE);
	return scene;
}
bool GameNormal::init() {
	if (!GameBase::init()) {
		return false;
	}
	UserDefault *ud = UserDefault::getInstance();
	StrRes * sr = StrRes::getInstance();
	this->_strScore = sr->getString(RSTR::score);
	this->_strBest = sr->getString(RSTR::best);
	this->_score = 0;
	this->_status = GAME_RUNNING;
	this->_best = ud->getIntegerForKey(SP_HS_CLASSIC, 0);
	initGame();
	this->_giveup->setEnabled(false);
	this->_mode = GAME_NORMAL;
	if (ud->getIntegerForKey(SP_TUTORIAL_CLASSIC, 1)) {
		startTutorial();
		return true;
	}
	if (gameStartNew == 0) {
		onLoadProgress();
	}
	else {
		setTiles(5, 6);
		setCandColors(0, 1);
	}
	return true;}

int GameNormal::checkGameOver() {
	int ret = 1;
	int i, j, n, nless1, m;
	for (i = 1; i < this->_xCount; ++i) {
		if (this->_tileColors[i] == this->_tileColors[i - 1]) {
			ret = 0;
			break;
		}
	}
	if (ret) {
		for (i = 1; i < this->_yCount; ++i) {
			n = i*this->_xCount;
			if (this->_tileColors[n] == this->_tileColors[n - this->_xCount]) {
				ret = 0;
				break;
			}
		}
	}
	if (ret) {
		for (i = 1; i < this->_yCount; ++i) {
			n = i * this->_xCount;
			nless1 = n - 1;
			m = (i - 1) * this->_xCount;
			for (j = 1; j < this->_xCount; ++j) {
				if (this->_tileColors[n + j] == this->_tileColors[nless1 + j] ||
					this->_tileColors[n + j] == this->_tileColors[m + j]) {
					ret = 0;
					break;
				}
			}
		}
	}
	return ret;
}

bool GameNormal::saveBest(char *str)  {
	UserDefault * ud = UserDefault::getInstance();
	StrRes *sr = StrRes::getInstance();
	sprintf(str, "%s: %d", sr->getString(RSTR::score), this->_score);
	if (this->_score > this->_best) {
		this->_best = this->_score;
		ud->setIntegerForKey(SP_HS_CLASSIC, this->_best);
		ud->flush();
		return true;
	}
	return false;
}

int GameNormal::updateScore(int n) {
	char str[8];
	int delta = 1;
	if (n < 0) {
		delta = n;
	}
	else if(n > 2) {
		delta = ((n - 2) << 1);
	}
	this->_score += delta;
	formatScore(str, this->_score);
	this->_laScore->setString(str);
	return delta;
}

void GameNormal::startTutorial() {
	this->_status = GAME_PAUSED;
	int i;
	char * data = new char[5 * 6];
	char fixed[] = {1, 2, 3, 0, 1,
	                0, 1, 1, 3, 0,
	                3, 0, 1, 2, 1,
	                2, 3, 0, 1, 2,
	                1, 3, 0, 2, 2,
	                0, 3, 2, 3, 0};

	char cand[] = { 0, 2, 0, 0, 1, 2, 3, 2 };
	for (i = 0; i < 30; ++i) {
		data[i] = fixed[i];
	}
	setTiles(5, 6, 4, data);
	setCandColors(0, 1, (char*)cand);
	this->schedule(schedule_selector(GameNormal::beginAutoSwipe), 0.1f, 0, 0.1f);
}
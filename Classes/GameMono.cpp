#include "GameMono.h"
#include "GameHome.h"
#include "SimpleAudioEngine.h"
#include "strres.h"

USING_NS_CC;

Scene* GameMono::createScene()
{
	auto scene = Scene::create();
	auto layer = GameMono::create();
	scene->addChild(layer, 0, TAG_GAMESCENE);
	return scene;
}
bool GameMono::init() {
	if (!GameBase::init()) {
		return false;
	}
	UserDefault *ud = UserDefault::getInstance();
	StrRes * sr = StrRes::getInstance();
	this->_strScore = sr->getString(RSTR::swipes);
	this->_strBest = sr->getString(RSTR::best);
	this->_score = 0;
	this->_status = GAME_RUNNING;
	this->_best = ud->getIntegerForKey(SP_HS_MONOCOLOR, 9999);
	initGame();
	this->_mode = GAME_MONOCLR;
	if (ud->getIntegerForKey(SP_TUTORIAL_MONO, 1)) {
		startTutorial();
		return true;
	}
	if (gameStartNew == 0) {
		onLoadProgress(3);
	}
	else {
		setTiles(5, 6, 3);
		setCandColors(0, 1);
	}
	return true;
}

int GameMono::checkGameOver() {
	int ret = 1;
	for (int i = 1; i < this->_xCount * this->_yCount; ++i) {
		if (this->_tileColors[i] != this->_tileColors[0]) {
			ret = 0;
			break;
		}
	}
	return ret;
}

bool GameMono::saveBest(char *str)  {
	UserDefault * ud = UserDefault::getInstance();
	StrRes *sr = StrRes::getInstance();
	sprintf(str, "%s: %d", sr->getString(RSTR::score), this->_score);
	if (this->_score < this->_best) {
		this->_best = this->_score;
		ud->setIntegerForKey(SP_HS_MONOCOLOR, this->_best);
		ud->flush();
		return true;
	}
	return false;
}

int GameMono::updateScore(int n) {
	char str[8];
	this->_score += 1;
	formatScore(str, this->_score);
	this->_laScore->setString(str);
	return 1;
}


void GameMono::startTutorial() {
	this->_status = GAME_PAUSED;
	int i;
	char * data = new char[5 * 6];
	char fixed[] = { 1, 2, 2, 0, 1,
		             0, 1, 1, 2, 0,
		             2, 0, 1, 2, 1,
		             2, 2, 0, 1, 1,
		             1, 2, 1, 2, 1,
		             0, 2, 2, 1, 0 };
	char cand[] = { 0, 2, 0, 0, 1, 2, 1, 2 };
	for (i = 0; i < 30; ++i) {
		data[i] = fixed[i];
	}
	setTiles(5, 6, 3, data);
	setCandColors(0, 1, (char *)cand);
	this->schedule(schedule_selector(GameMono::beginAutoSwipe), 0.1f, 0, 0.1f);
}

inline void GameMono::formatScore(char *str, int score) {
	if (score >= 9999) {
		sprintf(str, "::::");
	}
	else {
		sprintf(str, "%04d", score);
	}
}

void GameMono::onGiveUp(Ref * pSender) {
	Size size;
	float btnWidth = 220 * this->_xScale;
	float btnHeight = 72 * this->_xScale;
	float btnBorder = 2 * this->_xScale;
	float fntSize = 32 * this->_xScale;
	Color3B color1(255, 128, 128);
	this->_funcbar->setEnabled(false);
	this->_status = GAME_PAUSED;
	auto sr = StrRes::getInstance();
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
	auto bkg = Sprite::create();
	size = Director::getInstance()->getVisibleSize();
	Rect r;
	r.setRect(0, 0, 440.0f*this->_xScale, 200.0f*this->_xScale);
	auto layer = Sprite::create();
	layer->setColor(Color3B(0, 0, 0));
	layer->setTextureRect({ 0, 0, size.width, size.height });
	layer->setPosition(size.width / 2, size.height / 2);
	layer->setOpacity(128);
	this->addChild(layer, 1, 2990);
	bkg->setColor(COLOR_2);
	bkg->setTextureRect(r);
	bkg->runAction(Sequence::create(
		ScaleTo::create(0.00f, 0.1f),
		ScaleTo::create(0.2f, 1.1f),
		ScaleTo::create(0.1f, 1.0f), NULL));
	bkg->setPosition(size.width / 2, size.height / 2);
	layer->addChild(bkg);
	
	auto text = LabelTTF::create(sr->getString(RSTR::giveup_ask), "Arial", 32 * this->_xScale);
	text->setPosition(220.0f*this->_xScale, 140 * this->_xScale);
	bkg->addChild(text);
	
	auto yes = GameHome::createDialogButton(sr->getString(RSTR::giveup_yes), btnWidth, btnHeight, fntSize, btnBorder,
		COLOR_2, color1, 1, CC_CALLBACK_1(GameMono::dialogCallback, this, 1));
	auto no = GameHome::createDialogButton(sr->getString(RSTR::giveup_no), btnWidth, btnHeight, fntSize, btnBorder,
		COLOR_2, color1, 0, CC_CALLBACK_1(GameMono::dialogCallback, this, 0));
	yes->setAnchorPoint({ 0, 0 });
	no->setAnchorPoint({ 0, 0 });
	yes->setPosition(220 * this->_xScale, 0);
	no->setPosition(0, 0);
	auto options = Menu::create(yes, no, nullptr);
	options->setPosition(Point::ZERO);
	bkg->addChild(options);
}

void GameMono::dialogCallback(Ref *pSender, int answer) {
	int i;
	int colorCount[3] = {0, 0, 0};
    int temp,
		multi = 0;
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
	this->_funcbar->setEnabled(true);
	this->_status = GAME_RUNNING;
	this->removeChildByTag(2990);
	if (answer) {
		for (i = 0; i < this->_xCount * this->_yCount; ++i) {
			colorCount[this->_tileColors[i]]++;
		}
		if (colorCount[1] < colorCount[0]) {
			temp = colorCount[0];
			if (colorCount[2] < colorCount[1]) {
				colorCount[0] = colorCount[2];
				colorCount[2] = temp;
			}
			else {
				if (colorCount[2] < temp) {
					colorCount[0] = colorCount[1];
					colorCount[1] = colorCount[2];
					colorCount[2] = temp;
				}
				else {
					colorCount[0] = colorCount[1];
					colorCount[1] = temp;
				}
			}
		}
		else {
			if (colorCount[2] < colorCount[1]) {
				temp = colorCount[2];
				colorCount[2] = colorCount[1];
				if (temp < colorCount[0]) {
					colorCount[1] = colorCount[0];
					colorCount[0] = temp;
				}
				else {
					colorCount[1] = temp;
				}
			}
		}
		if (this->_score < 4) {
			temp = 128;
		}
		else {
			temp = this->_xCount * this->_yCount - colorCount[2];
		}
		this->_score += (temp * 4 + colorCount[0] * 4 + colorCount[1] * 8);
		onGameOver();
	}
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void GameMono::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event){
	if(keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE)  {
		auto dialog = this->getChildByTag(2990);
		if(dialog!=nullptr) {
			dialogCallback(nullptr, 0);
		}
		else {
			GameBase::onKeyReleased(keyCode, event);
		}
	}
}
#endif
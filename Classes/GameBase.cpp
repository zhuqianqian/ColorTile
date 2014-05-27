#include "GameBase.h"
#include "GameHome.h"
#include "SimpleAudioEngine.h"
#include "strres.h"

#define DEFAULT_MAX_STACK	(2)

#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
#include "gpghelper.h"
#endif /* CC_TARGET_PLATFORM */

USING_NS_CC;

Color3B clrList[] = {
	COLOR_0, COLOR_1, COLOR_2, COLOR_3,
	COLOR_4, COLOR_5, COLOR_6
};

#define FONTSIZE  (21)
#define CREATE_CAND_TILE(var, rect, x, y, i)	\
	(var) = Sprite::create(); \
	(var)->setTextureRect((rect)); \
	(var)->setPosition((x), (y)); \
	this->_candidates[(i)] = (var); \
	this->addChild((var));

#define ADD_LABEL(var, text, size, x, y, anchor) \
	(var) = LabelTTF::create((text), "Arial", (size)); \
	(var)->setColor(Color3B::BLACK); \
	(var)->setAnchorPoint((anchor)); \
	(var)->setPosition((x), (y)); \
	this->addChild((var));

int GameBase::gameStartNew = 1;

bool GameBase::init() {
	this->_useGoogleGame = UserDefault::getInstance()->getBoolForKey(SP_ENABLE_GOOGLE, false);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	this->setKeypadEnabled(true);
#endif
	return LayerColor::initWithColor(Color4B(0xff, 0xff, 0xff, 0xff));
}

bool GameBase::initGame() {
	this->_wndSize = Director::getInstance()->getVisibleSize();
	this->_xScale = this->_wndSize.width / DESIGN_WIDTH;
	this->_dpScale = 1.0f;
	if (this->_xScale < 0.8) {
		this->_dpScale = 2.0f / 3.0f;
	}
	else if (this->_xScale > 1.25f && this->_xScale < 1.75f) {
		this->_dpScale = 1.5;
	}
	else if (this->_xScale > 1.75f && this->_xScale < 2.25f) {
		this->_dpScale = 2.0f;
	}
	this->_tileInterval = 6 * this->_xScale;
	this->_recIndex = 0;
	setHeader();
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameBase::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(GameBase::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameBase::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

int GameBase::updateScore(int n) { return 0; }

bool GameBase::setTiles(int x, int y, int colors, char *tiles) {
	int xTile, yTile;
	int i, j, n;
	int posX, posY;
	this->_xCount = x;
	this->_yCount = y;
	this->_nColors = colors;
	Rect rect;
	int boardHeight = this->_wndSize.height - this->_headerHeight;
	this->_xBoard = this->_yBoard = this->_tileInterval;
	xTile = (this->_wndSize.width - this->_tileInterval) / x - this->_tileInterval;
	yTile = (this->_wndSize.height - this->_headerHeight - this->_tileInterval) / y - this->_tileInterval;
	this->_tileSize = xTile > yTile ? yTile : xTile;
	if (xTile == this->_tileSize) {
		this->_xBoard = this->_tileInterval;
		this->_yBoard += (boardHeight - (xTile + this->_tileInterval) * y) / 2;
		this->_yBoard -= this->_tileInterval / 2;
	}
	else {
		this->_xBoard += (this->_wndSize.width - (yTile + this->_tileInterval) * x) / 2;
		this->_xBoard -= this->_tileInterval / 2;
		this->_yBoard += this->_tileInterval;
	}
	this->_maxStackCap = DEFAULT_MAX_STACK;
	this->_stackData = new char[8 * (x * y + 9)];
	for (i = 0; i < 8; ++i) {
		this->_stackPtrs[i] = this->_stackData + (x*y + 9) * i;
	}
	memset(this->_stackData, 0, 8 * (x * y + 9));
	this->_stackPointer = 0;
	this->_stackCap = 0;
	if (tiles != nullptr) {
		this->_tileColors = tiles;
	}
	else {
		this->_tileColors = new char[x*y];
		for (i = 0; i < x*y; ++i) {
			this->_tileColors[i] = rand() % colors;
		}
	}
	this->_swipeRecords= new int[x*y];
	this->_tileSprites = new Sprite *[x*y];
	posY = this->_yBoard + this->_tileSize / 2;
	rect.setRect(0, 0, this->_tileSize, this->_tileSize);
	for (i = 0; i < y; ++i) {
		n = i * x;
		posX = this->_xBoard + this->_tileSize / 2;
		for (j = 0; j < x; ++j) {
			auto s = Sprite::create();
			s->setColor(clrList[this->_tileColors[n + j]]);			
			s->setPosition(posX, posY);
			s->setTextureRect(rect);
			this->_tileSprites[n + j] = s;
			this->addChild(s);
			posX += this->_tileSize + this->_tileInterval;
		}
		posY += this->_tileSize + this->_tileInterval;
	}
	return true;
}

void GameBase::setHeader() {
	StrRes* sr;
	float fntSize;
	float x, y;
	int i, textHeight;
	char str[8];
	float marginTop = this->_tileInterval * 2;
	float candInterval = 2 * this->_xScale;
	float candTileSize = 36 * this->_xScale;
	float candSpaceTotal = candInterval + candTileSize;
	float middleLine, room;
	Rect rect;
	Sprite *s;
	LabelTTF * label;
	Point anchor = {0.0f, 1.0f};
	Point anchorDigits = { 0.0f, 0.5f };
	sr = StrRes::getInstance();
	rect.setRect(0, 0, candTileSize, candTileSize);
	
	x = this->_wndSize.width - 4 * candSpaceTotal - this->_tileInterval;
	y = this->_wndSize.height - marginTop;
	fntSize = FONTSIZE*this->_xScale;
	ADD_LABEL(label, sr->getString(RSTR::next_prev), fntSize, x, y, anchor);
	textHeight = label->getContentSize().height;
	middleLine = y - (textHeight + candTileSize) / 2 - candInterval;
	x = this->_wndSize.width - this->_tileInterval - candTileSize / 2;
	y -= (textHeight + candInterval * 2 + candTileSize);
	y -= candTileSize / 2;
	for (i = 7; i > 3; --i) {
		CREATE_CAND_TILE(s, rect, x, y, i);
		x -= candSpaceTotal;
	}
	room = x;
	y += candSpaceTotal;
	x = this->_wndSize.width - this->_tileInterval - candTileSize / 2;
	for (; i >= 0; --i) {
		CREATE_CAND_TILE(s, rect, x, y, i);
		x -= candSpaceTotal;
	}

	x = marginTop;
	y = this->_wndSize.height - marginTop - 6 * this->_xScale;
	ADD_LABEL(label, this->_strBest, fntSize, x, y, anchor);
	formatScore(str, this->_best);
	this->_laBest = LabelAtlas::create(str, "digits.png", 18, 32, '0');
	this->_laBest->setAnchorPoint(anchor);
	this->_laBest->setScale(this->_xScale, this->_xScale);
	x += label->getContentSize().width + this->_tileInterval;
	this->_laBest->setPosition(x, y);
	this->addChild(this->_laBest);
	x += 96 * this->_xScale;
	ADD_LABEL(label, this->_strScore, fntSize, x, y, anchor);
	formatScore(str, this->_score);
	this->_laScore = LabelAtlas::create(str, "digits.png", 18, 32, '0');
	this->_laScore->setAnchorPoint(anchor);
	this->_laScore->setScale(this->_xScale, this->_xScale);
	x += label->getContentSize().width + this->_tileInterval;
	this->_laScore->setPosition(x, y);
	this->addChild(this->_laScore);
	
	auto backItem = MenuItemImage::create("back0.png", "back1.png", CC_CALLBACK_1(GameBase::onBack, this));
	auto undo = MenuItemImage::create("undo0.png", "undo1.png", "undo2.png", CC_CALLBACK_1(GameBase::onUndo, this));
	auto giveup = MenuItemImage::create("giveup0.png", "giveup1.png", "giveup2.png",CC_CALLBACK_1(GameBase::onGiveUp, this));
	sprintf(str, "%d", DEFAULT_MAX_STACK);
	auto labelStack = LabelTTF::create(str, "Arial", 15 * this->_xScale);
	labelStack->setColor(Color3B::GRAY);
	this->addChild(labelStack, 2);
	this->_maxStackLabel = labelStack;
	this->_undo = undo;
	this->_giveup = giveup;
	undo->setEnabled(false);
	backItem->setScale(this->_xScale, this->_xScale); 
	undo->setScale(this->_xScale, this->_xScale);
	giveup->setScale(this->_xScale, this->_xScale);
	x = marginTop;
	y = middleLine - candInterval * 4;
	backItem->setAnchorPoint(anchor);
	backItem->setPosition(x, y);
	undo->setPosition(x + room / 3, y);
	undo->setAnchorPoint(anchor);
	labelStack->setAnchorPoint(anchor);
	labelStack->setPosition(x + room / 3 + 48 * this->_xScale, y);
	giveup->setPosition(x + room * 0.67, y);
	giveup->setAnchorPoint(anchor);
	auto funcbar = Menu::create(backItem, undo, giveup, NULL);
	funcbar->setPosition(Point::ZERO);
	this->addChild(funcbar);
	this->_funcbar = funcbar;
	this->_headerHeight = marginTop + 2 * (candSpaceTotal) + textHeight;
}

void GameBase::setCandColors(int n, int init, char *data) {
	int i;
	if (init) {
		if (data != nullptr) {
			for (i = 0; i < 8; ++i) {
				this->_candColor[i] = data[i];
				this->_candidates[i]->setColor(clrList[this->_candColor[i]]);
			}
		}
		else {
			for (i = 0; i < 8; ++i) {
				this->_candColor[i] = rand() % this->_nColors;
				this->_candidates[i]->setColor(clrList[this->_candColor[i]]);
			}
		}
	}
	else {
		for (i = n; i < 8; ++i) {
			this->_candColor[i - n] = this->_candColor[i];
			this->_candidates[i - n]->setColor(clrList[this->_candColor[i]]);
		}
		if (n > 8) { n = 8; }
		for (i = 0; i < n; ++i) {
			this->_candColor[7 - i] = rand() % this->_nColors;
			this->_candidates[7 - i]->setColor(clrList[this->_candColor[7 - i]]);
		}
	}
}

void GameBase::onBack(Ref * pSender) {
	onSaveProgress();
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
	auto homeScene = GameHome::createScene();
	auto reScene = TransitionFadeBL::create(1.0, homeScene);
	Director::getInstance()->replaceScene(reScene);
}

bool GameBase::onTouchBegan(Touch *touch, Event *event) {
	int i, n;
	Sprite *s = NULL;
	if (this->_status != GAME_RUNNING) {
		return true;
	}
	Point touchLoc = this->convertTouchToNodeSpace(touch);

	if (this->_regFlag && this->_recIndex) {
		onTileUpdate();
	}
	s = NULL;
	for (i = 0; i < this->_xCount * this->_yCount; ++i) {
		if (this->_tileSprites[i]->getBoundingBox().containsPoint(touchLoc)) {
			s = this->_tileSprites[i];
			break;
		}
	}
	if (s != NULL) {
		n = i;
		for (i = 0; i < this->_recIndex; ++i)
		{
			this->_tileSprites[this->_swipeRecords[i]]->runAction(
				ScaleTo::create(0.1f, 1.0f));
		}
		this->_recIndex = 0;
		s->runAction(ScaleTo::create(0.1f, 0.85f));
		this->_swipeRecords[this->_recIndex] = n;
		this->_recIndex++;
	}
	return true;
}

void GameBase::onTouchMoved(Touch * touch, Event * event) {
	int i, n;
	Sprite *s = NULL;
	if (this->_status != GAME_RUNNING) {
		return;
	}
	Point touchLoc = this->convertTouchToNodeSpace(touch);
	for (i = 0; i < this->_xCount * this->_yCount; ++i) {
		if (this->_tileSprites[i]->getBoundingBox().containsPoint(touchLoc)) {
			s = this->_tileSprites[i];
			break;
		}
	}
	n = this->_recIndex - 1;
	if (s != NULL) {
		if (i != this->_swipeRecords[n]) {
			if (n >= 0 &&
				this->_tileColors[this->_swipeRecords[n]] == this->_tileColors[i] &&
				((i + 1) == this->_swipeRecords[n] ||
				(i - 1) == this->_swipeRecords[n] ||
				(i + this->_xCount) == this->_swipeRecords[n] ||
				(i - this->_xCount) == this->_swipeRecords[n])) {
				this->_swipeRecords[this->_recIndex] = i;
				this->_recIndex++;
				s->runAction(ScaleTo::create(0.1f, 0.85f));
				if (n > 0 && i == this->_swipeRecords[n - 1])  {
					this->_recIndex = n;
					this->_tileSprites[this->_swipeRecords[n]]->runAction(ScaleTo::create(0.1f, 1.0f));
				}
			}
			else {
				n = i;
				for (i = 0; i < this->_recIndex; ++i) {
					this->_tileSprites[this->_swipeRecords[i]]->runAction(
						ScaleTo::create(0.1f, 1.0f));
				}
				this->_tileSprites[n]->runAction(ScaleTo::create(0.1f, 0.85f));
				this->_swipeRecords[0] = n;
				this->_recIndex = 1;
			}
		}
	}
}

void GameBase::onTouchEnded(Touch *touch, Event * event) {
	int i, n;
	Sprite *s = NULL;
	if (this->_status != GAME_RUNNING) {
		return;
	}
	if (this->_recIndex > 1) {
		n = this->_recIndex - 1;
		auto a5 = CallFunc::create(CC_CALLBACK_0(GameBase::onTileUpdate, this));
		for (i = 0; i < n; ++i) {
			auto a0 = ScaleTo::create(0.05f, 1.0f);
			auto a1 = FadeOut::create(0.1f);
			auto a2 = FadeIn::create(0.1f);
			auto a3 = FadeOut::create(0.1f);
			auto a4 = FadeOut::create(0.1f);
			auto seq = Sequence::create(a0, a1, a2, a3, a4, NULL);
			this->_tileSprites[this->_swipeRecords[i]]->stopAllActions();
			this->_tileSprites[this->_swipeRecords[i]]->runAction(seq);
		}
		auto a0 = ScaleTo::create(0.05f, 1.0f);
		auto a1 = FadeOut::create(0.1f);
		auto a2 = FadeIn::create(0.1f);
		auto a3 = FadeOut::create(0.1f);
		auto a4 = FadeOut::create(0.1f);
		auto seq_final = Sequence::create(a0, a1, a2, a3, a4, a5, NULL);
		this->_regFlag = 1;
		this->_tileSprites[this->_swipeRecords[i]]->stopAllActions();
		this->_tileSprites[this->_swipeRecords[i]]->runAction(seq_final);
	}
	else {
		if (this->_recIndex > 0) {
			s = this->_tileSprites[this->_swipeRecords[0]];
			s->runAction(ScaleTo::create(0.1f, 1.0f));
			this->_recIndex = 0;
		}
	}
}

void GameBase::onTileUpdate() {
	int n, i;
	char str[4];
	Sprite *s = NULL;
	n = this->_recIndex;
	if (n > 16 && this->_maxStackCap < 9) {
		sprintf(str, "%d", ++this->_maxStackCap);
		this->_maxStackLabel->setString(str);
		if (this->_maxStackCap) {
			this->_maxStackLabel->setVisible(true);
		}
	}
	pushStack(updateScore(n));
	if (this->_regFlag) {
		for (i = 0; i < n && i < 8; ++i) {
			this->_tileColors[this->_swipeRecords[i]] = this->_candColor[i];
			s = this->_tileSprites[this->_swipeRecords[i]];
			s->stopAllActions();
			s->setColor(clrList[this->_tileColors[this->_swipeRecords[i]]]);
			s->runAction(FadeIn::create(0.1f));
		}
		for (; i < n; ++i) {
			this->_tileColors[this->_swipeRecords[i]] = rand() % this->_nColors;
			s = this->_tileSprites[this->_swipeRecords[i]];
			s->stopAllActions();
			s->setColor(clrList[this->_tileColors[this->_swipeRecords[i]]]);
			s->runAction(FadeIn::create(0.1f));
		}
		setCandColors(n);
		this->_recIndex = 0;
		this->_regFlag = 0;
	}
	if (checkGameOver()) {
		onGameOver();
	}
}

void GameBase::onGameOver() {
	StrRes *sr = StrRes::getInstance();
	int bShowNewBest = 0;
	Rect r;
	int x, y, clr;
	char str[32];
	this->_status = GAME_OVER;
	Point anchor;
	this->_funcbar->setEnabled(false);
	x = this->_xCount * (this->_tileSize + this->_tileInterval) - this->_tileInterval;
	y = this->_yCount * (this->_tileSize + this->_tileInterval) - this->_tileInterval;
	r.setRect(0, 0, x, y);
	clr = (this->_yCount / 2) * this->_xCount + this->_xCount / 2;
	auto bkg = Sprite::create();
	bkg->setColor(clrList[this->_tileColors[clr]]);
	bkg->setTextureRect(r);
	bkg->setPosition(this->_xBoard + x / 2, this->_yBoard + y / 2);
	bkg->setOpacity(160);
	this->addChild(bkg, 1);
	bkg->runAction(Sequence::create(
		ScaleTo::create(0.005f, 0.0f),
		ScaleTo::create(0.22f, 1.1f),
		ScaleTo::create(0.083f, 1.0f),
		NULL));
	auto title = LabelTTF::create(sr->getString(RSTR::game_over), "Arial", 48 * this->_xScale);
	title->setPosition(x / 2, 0.75f * y);
	bkg->addChild(title);
	bShowNewBest = saveBest(str);
	auto score = LabelTTF::create(str, "Arial", 60 * this->_xScale);
	LabelTTF * newBest;
	if (bShowNewBest) {
		newBest = LabelTTF::create(sr->getString(RSTR::new_best), "Arial", 36 * this->_xScale);
	}
	else {
		sprintf(str, "%s: %d", sr->getString(RSTR::best), this->_best);
		newBest = LabelTTF::create(str, "Arial", 36 * this->_xScale);
	}
	anchor.setPoint(0.5f, 1.0f);
	newBest->setAnchorPoint(anchor);
	newBest->setPosition(x / 2, 0.5*y);
	bkg->addChild(newBest);
	anchor.setPoint(0.5f, 0.0f);
	score->setAnchorPoint(anchor);
	score->setPosition(x / 2, 0.5*y);
	bkg->addChild(score);
	auto menuBack = MenuItemFont::create(sr->getString(RSTR::back), CC_CALLBACK_1(GameBase::onBack, this));
	auto menuAgain = MenuItemFont::create(sr->getString(RSTR::retry), CC_CALLBACK_1(GameBase::restartGame, this));
	auto menuShare = MenuItemFont::create(sr->getString(RSTR::share), CC_CALLBACK_1(GameBase::onShare, this));
	menuBack->setPosition(x * 0.2f, 0.25 * y);
	menuAgain->setPosition(x / 2, 0.25 * y);
	menuShare->setPosition(x * 0.8f, 0.25 * y);
	auto menu = Menu::create(menuBack, menuAgain, menuShare, NULL);
	menu->setPosition(Point::ZERO);
	bkg->addChild(menu);
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
	if(this->_useGoogleGame) {
		if (::isSignedIn()) {
			::submitScore(this->_mode, this->_score);
		}
		else {
			//::beginUserInitiatedSigned();
		}
	}
#endif /* CC_TARGET_PLATFORM */
	this->_gameOverBkg = bkg;
}

void GameBase::restartGame(Ref* pSender) {
	char str[4];
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
	this->_recIndex = 0;
	this->_gameOverBkg->runAction(Sequence::create(
		FadeIn::create(0.25f),
		CallFunc::create(CC_CALLBACK_0(GameBase::removeGameOverDialog, this)),
		NULL));
	this->_funcbar->setEnabled(true);
	this->_stackCap = 0;
	this->_maxStackCap = DEFAULT_MAX_STACK;
	this->_stackPointer = 0;
	this->_undo->setEnabled(false);
	sprintf(str, "%d", this->_maxStackCap);
	this->_maxStackLabel->setString(str);
}

void GameBase::removeGameOverDialog() {
	int n = this->_xCount * this->_yCount;
	this->_status = GAME_RUNNING;
	this->removeChild(this->_gameOverBkg);
	for (int i = 0; i < n; ++i) {
		this->_tileColors[i] = rand() % this->_nColors;
		this->_tileSprites[i]->setColor(clrList[this->_tileColors[i]]);
	}
	setCandColors(0, 1);
	reset();
}

void GameBase::reset() {
	char str[8];
	this->_score = 0;
	this->_laScore->setString("0000");
	formatScore(str, this->_best);
	this->_laBest->setString(str);
}

void GameBase::onShare(Ref* pSender){
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
	doShare(this->_mode, this->_best);
#endif /* CC_TARGET_PLATFORM */
}

void GameBase::onSaveProgress() {
	auto ud = UserDefault::getInstance();
	int i;
	char str[64] = { 0 };
	ud->setIntegerForKey(SP_GAME_STATUS, this->_status);
	ud->setIntegerForKey(SP_GAME_MODE, this->_mode);
	if (this->_status == GAME_RUNNING) {
		ud->setIntegerForKey(SP_GAME_SCORE, this->_score);
		ud->setIntegerForKey(SP_X_COUNT, this->_xCount);
		ud->setIntegerForKey(SP_Y_COUNT, this->_yCount);
		for (i = 0; i < this->_xCount * this->_yCount; ++i) {
			str[i] = this->_tileColors[i] + '0';
		}
		ud->setStringForKey(SP_GAME_DATA, str);
		for (i = 0; i < 8; ++i) {
			str[i] = this->_candColor[i] + '0';
		}
		str[i] = 0;
		ud->setStringForKey(SP_CAND_COLORS, str);
	}
	ud->flush();
}

void GameBase::onLoadProgress(int nColors) {
	auto ud = UserDefault::getInstance();
	char str[8];
	int i, x, y;
	char *data;
	std::string cstr;
	this->_status = ud->getIntegerForKey(SP_GAME_STATUS, GAME_OVER);
	if (this->_status == GAME_RUNNING) {
		this->_score = ud->getIntegerForKey(SP_GAME_SCORE, 0);
		x = ud->getIntegerForKey(SP_X_COUNT, 5);
		y = ud->getIntegerForKey(SP_Y_COUNT, 6);
		cstr = ud->getStringForKey(SP_GAME_DATA);
		data = new char[x * y];
		for (i = 0; i < x*y; ++i) {
			data[i] = cstr[i] - '0';
		}
		setTiles(x, y, nColors, data);
		cstr = ud->getStringForKey(SP_CAND_COLORS, "01230123").c_str();
		for (i = 0; i < 8; ++i) {
			this->_candColor[i] = cstr[i] - '0';
			this->_candidates[i]->setColor(clrList[this->_candColor[i]]);
		}
		formatScore(str, this->_score);
		this->_laScore->setString(str);
	}
}

void GameBase::beginAutoSwipe(float dt) {
	float x, y;
	Point pos;
	int ids[] = {RSTR::tutorial0, RSTR::tuturial2};
	x = this->_xCount * (this->_tileSize + this->_tileInterval) - this->_tileInterval;
	y = this->_yCount * (this->_tileSize + this->_tileInterval) - this->_tileInterval;
	
	Sprite * textBkg = Sprite::create();
	textBkg->setTextureRect({ 0, 0, x, y });
	textBkg->setPosition(this->_xBoard + x / 2, this->_yBoard + y / 2);
	textBkg->setColor(Color3B::GRAY);
	textBkg->setOpacity(128);
	auto label = LabelTTF::create(StrRes::getInstance()->getString(ids[this->_mode]), "Arial", 36 * this->_xScale);
	this->addChild(textBkg, 1, 2991);
	this->_tileSprites[22]->getPosition(&x, &y);
	pos.x = x; pos.y = y - this->_yBoard;
	label->setPosition (-x, pos.y);
	textBkg->addChild(label);
	label->runAction(Sequence::create(DelayTime::create(0.3f), MoveTo::create(0.3f, pos),
		DelayTime::create(5.5f), FadeOut::create(0.3f), NULL));
	auto seq = Sequence::create(DelayTime::create(1.5f), 
		                        FadeOut::create(0.4f),
		                        FadeIn::create(0.2f),
								FadeOut::create(0.4f),
								FadeIn::create(0.2f),
								CallFunc::create(CC_CALLBACK_0(GameBase::tutorialNext, this)), // 2.2seconds
								NULL);
	this->_tileSprites[6]->runAction(seq);
	this->_tileSprites[7]->runAction(seq->clone());
	this->_tileSprites[12]->runAction(seq->clone());
}

void GameBase::tutorialNext() {
	float x, y;
	auto finger = Sprite::create("finger.png");
	finger->setScale(this->_xScale, this->_xScale);
	finger->setAnchorPoint({ 0.5f, 1.0f });
	this->addChild(finger, 1, 2992);
	this->_tileSprites[6]->getPosition(&x, &y);
	auto m0 = MoveTo::create(0.0f, { x, y });
	finger->setPosition(x, y);
	this->_tileSprites[7]->getPosition(&x, &y);
	auto m1 = MoveTo::create(1.0f, {x, y});
	this->_tileSprites[12]->getPosition(&x, &y);
	auto m2 = MoveTo::create(1.0f, {x, y});
	finger->runAction(Sequence::create(
		FadeIn::create(0.4f),
		m1, m2, //DelayTime::create(0.5f), 
		FadeOut::create(0.4f), // Total:0.4+1.0+1.0 + 0.4 = 2.8
		DelayTime::create(1.2f),
		CallFunc::create(CC_CALLBACK_0(GameBase::tutorialComplete, this)), // 4.0
		NULL));
	auto scale0 = ScaleTo::create(0.1f, 0.85f);
	auto scale1 = ScaleTo::create(0.05f, 1.0f);
	auto fadein = FadeIn::create(0.1f);
	auto fadeout = FadeOut::create(0.1f);
	this->_swipeRecords[0] = 6;
	this->_swipeRecords[1] = 7;
	this->_swipeRecords[2] = 12;
	this->_recIndex = 3;
	this->_regFlag = 1;
	this->_tileSprites[6]->runAction(Sequence::create(
		DelayTime::create(0.41f), scale0, 
		DelayTime::create(2.4f), scale1,
		fadeout, fadein, fadeout, fadein,
		NULL));
	this->_tileSprites[7]->runAction(Sequence::create(
		DelayTime::create(1.01f), scale0,
		DelayTime::create(1.8f), scale1,
		fadeout, fadein, fadeout, fadein,
		NULL));
	this->_tileSprites[12]->runAction(Sequence::create(
		DelayTime::create(2.01f), scale0,
		DelayTime::create(0.8f), scale1,
		fadeout, fadein, fadeout, fadein,
		CallFunc::create(CC_CALLBACK_0(GameBase::onTileUpdate, this)),
		NULL));
}

void GameBase::tutorialComplete() {
	float x, y;
	
	auto bkg = getChildByTag(2991);
	auto label = LabelTTF::create(StrRes::getInstance()->getString(RSTR::tuturial1),
		"Arial", 48 * this->_xScale);
	label->setOpacity(0);
	bkg->addChild(label);
	this->_tileSprites[22]->getPosition(&x, &y);
	label->setPosition(x, y - this->_yBoard);
	label->runAction(FadeIn::create(0.3f));
	bkg->runAction(Sequence::create(
		DelayTime::create(0.8f), 
		FadeOut::create(0.5f), 
		CallFunc::create(CC_CALLBACK_0(GameBase::tutorialCleanup, this)),
		NULL));
}

void GameBase::tutorialCleanup() {
	this->removeChildByTag(2991, true);
	this->removeChildByTag(2992, true);
	this->_status = GAME_RUNNING;
	UserDefault * ud = UserDefault::getInstance();
	if (this->_mode == GAME_NORMAL) {
		ud->setIntegerForKey(SP_TUTORIAL_CLASSIC, 0);
	}
	else {
		ud->setIntegerForKey(SP_TUTORIAL_MONO, 0);
	}
	ud->flush();
}

void GameBase::pushStack(int n) {
	static int nTotal = this->_xCount * this->_yCount;
	char *data;
	if (!this->_maxStackCap) {
		return;
	}
	if (!this->_stackCap) {
		this->_undo->setEnabled(true);
		this->_maxStackLabel->setColor(Color3B::BLACK);
	}
	this->_stackCap+=1;
	if(this->_maxStackCap < this->_stackCap) {
		this->_stackCap -= 1;
	}
	data = this->_stackPtrs[this->_stackPointer];
	memcpy(data, this->_tileColors, nTotal);
	memcpy(data + nTotal, this->_candColor, 8);
	*(data + nTotal + 8) = (char)n;
	this->_stackPointer += 1;
	if (this->_stackPointer >= this->_stackCap) {
		this->_stackPointer = 0;
	}
}

void GameBase::popStack() {
	char str[4];
	char *data;
	static int nTotal = this->_xCount * this->_yCount;
	if (!this->_stackCap) {
		return;
	}
	this->_stackPointer--;
	if(this->_stackPointer < 0) {
		this->_stackPointer = this->_stackCap-1;
	}
	data = this->_stackPtrs[this->_stackPointer];
	memcpy(this->_tileColors, data, nTotal);
	memcpy(this->_candColor, data + nTotal,8);
	updateScore((-1) * *(data + nTotal + 8));
	this->_stackCap--;
	if (!this->_stackCap) {
		this->_undo->setEnabled(false);
		this->_maxStackLabel->setColor(Color3B::GRAY);
	}
	this->_maxStackCap--;
	if (this->_maxStackCap) {
		sprintf(str, "%d", this->_maxStackCap);
		this->_maxStackLabel->setString(str);
	}
	else {
		this->_maxStackLabel->setVisible(false);
	}
}

void GameBase::onUndo(Ref *pSender) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_BTN);
	popStack();
	int i;
	static int nTotal = this->_xCount * this->_yCount;
	for(i = 0; i < nTotal; ++i) {
		this->_tileSprites[i]->setColor(clrList[this->_tileColors[i]]);
	}
	for(i =0; i < 8; ++i) {
		this->_candidates[i]->setColor(clrList[this->_candColor[i]]);
	}
}

void GameBase::onGiveUp(Ref *pSender) {
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void GameBase::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE) {
		onBack(nullptr);
	}
}
#endif

int GameBase::checkGameOver() { return 0; }
bool GameBase::saveBest(char *str) { return false; }
void GameBase::formatScore(char *str, int score) {
	sprintf(str, "%04d", score);
}
void GameBase::startTutorial() {}


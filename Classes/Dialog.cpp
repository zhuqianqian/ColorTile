#include "Dialog.h"

USING_NS_CC;
const Color3B __clrbkg(145, 151, 163);
const Color3B __clrtxt(255, 255, 255);
const Color3B __clrdrk( 83,  88, 100);

namespace z299studio {

    Dialog * __instance;

    MenuItemSprite* Dialog::createDialogButton(const char *text,
        float width, float height,
        float fntSize, float border,
        const Color3B normal,
        const Color3B pressed,
        const int style,
        const ccMenuCallback &callback)
    {
        Rect r1, r2;
        r1.setRect(0, 0, width, height);
        if (style == 1) {
            r2.setRect(border, 0, width, height - border);
        }
        else if (style == 0) {
            r2.setRect(0, 0, width - border, height - border);
        }

        auto bkgNormal = Sprite::create();
        auto bkgPressed = Sprite::create();
        auto rectNormal = Sprite::create();
        auto rectPressed = Sprite::create();
        auto label = LabelTTF::create(text, "Arial", fntSize);
        bkgNormal->setColor(__clrtxt);
        bkgPressed->setColor(__clrtxt);
        bkgNormal->setTextureRect(r1);
        bkgPressed->setTextureRect(r1);
        rectNormal->setColor(normal);
        rectNormal->setTextureRect(r2);
        rectPressed->setColor(pressed);
        rectPressed->setTextureRect(r2);
        label->setPosition(width / 2, height / 2);
        rectNormal->setAnchorPoint({ 0, 0 });
        rectPressed->setAnchorPoint({ 0, 0 });
        rectNormal->setPosition(0, 0);
        rectPressed->setPosition(0, 0);
        rectNormal->addChild(label);
        rectPressed->addChild(label);
        bkgNormal->addChild(rectNormal);
        bkgPressed->addChild(rectPressed);

        return MenuItemSprite::create(bkgNormal, bkgPressed, callback);
    }

    Dialog* Dialog::build() {
        auto layer = Dialog::create();
        return dynamic_cast<Dialog *> (layer);
    }

    Dialog* Dialog::getInstance() {
        return __instance;
    }

    bool Dialog::init() {
        if (!LayerColor::initWithColor(Color4B(0, 0, 0, 128))) {
            return false;
        }
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = CC_CALLBACK_2(Dialog::onTouchBegan, this);
        listener->onTouchMoved = CC_CALLBACK_2(Dialog::onTouchMoved, this);
        listener->onTouchEnded = CC_CALLBACK_2(Dialog::onTouchEnded, this);
        listener->onTouchCancelled = CC_CALLBACK_2(Dialog::onTouchCancelled, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);     
        this->_menu = nullptr;
        this->_dialog = nullptr;
        this->_scale = 1.0;
        this->_dc = nullptr;
        //this->_title = nullptr;
        //this->_text = nullptr;
        //this->_yes = nullptr;
        //this->_neutral = nullptr;
        __instance = nullptr;
        return true;
    }

    Dialog * Dialog::setContentScale(float scale) {
        this->_scale = scale;
        return this;
    }

    void Dialog::show(const char * title, const char * text, const DialogCallback &callback,
        const char * yes, const char * no, const char *neutral) {
        Rect r1, r2;
        float height, margin;
        float buttonWidth, buttonHeight, buttonBorder, buttonSize;
        Size wndSize;
        int itemCount, i;
        const char * ptrs[3], *args[3] = { no, neutral, yes };
        int styles[3];
        int answers[3];
        LabelTTF * labelTitle, *labelText;
        wndSize = Director::getInstance()->getVisibleSize();
        wndSize.width -= 24 * this->_scale;
        MenuItemSprite * mis[3] = { nullptr, nullptr, nullptr };
        height = 0;
        margin = 24 * this->_scale;
        this->_dialog = Sprite::create();

        itemCount = 0;
        for (i = 0; i < 3; i++) {
            if (args[i]) {
                ptrs[itemCount] = args[i];
                styles[itemCount] = itemCount > 0 ? 1 : 0;
                answers[itemCount] = i;
                itemCount++;
            }
        }
        if (itemCount < 1) { return; }
        buttonWidth = wndSize.width / itemCount;
        buttonHeight = 64 * this->_scale;
        buttonBorder = 1 * this->_scale;
        buttonSize = 30 * this->_scale;
        for (i = 0; i < itemCount; ++i) {
            mis[i] = createDialogButton(ptrs[i], buttonWidth, buttonHeight, buttonSize, buttonBorder,
                __clrbkg, __clrdrk, styles[i], CC_CALLBACK_1(Dialog::dialogCallback, this, answers[i]));
            mis[i]->setAnchorPoint(Point::ZERO);
            mis[i]->setPosition(0 + buttonWidth * i, 0);
        }
        this->_menu = Menu::create(mis[0], mis[1], mis[2], nullptr);
        this->_menu->setPosition(Point::ZERO);
        this->_dialog->addChild(this->_menu);
        height += buttonHeight;

        labelText = LabelTTF::create(text, "Arial", 32 * _scale);
        labelText->setPosition(wndSize.width / 2, buttonHeight + labelText->getContentSize().height / 2 + margin);
        this->_dialog->addChild(labelText);
        height += labelText->getContentSize().height + 2 * margin;
        if (title != nullptr) {
            labelTitle = LabelTTF::create(title, "Arial", 36 * _scale);
            this->_dialog->addChild(labelTitle);
            labelTitle->setPosition(wndSize.width / 2, height + labelTitle->getContentSize().height / 2);
            height += labelTitle->getContentSize().height + margin;
        }
        r1.setRect(0, 0, wndSize.width, height);
        this->_dialog->setTextureRect(r1);
        this->_dialog->setColor(__clrbkg);
        this->_dialog->setPosition(wndSize.width / 2 + 16 * this->_scale, wndSize.height / 2);
        this->_dialog->runAction(Sequence::create(
            ScaleTo::create(0.00f, 0.1f),
            ScaleTo::create(0.2f, 1.1f),
            ScaleTo::create(0.1f, 1.0f), NULL));
        this->addChild(this->_dialog);
        Director::getInstance()->getRunningScene()->addChild(this);
        this->_dc = callback;
        __instance = this;
    }

    void Dialog::dialogCallback(Ref *pSender, int answer) {
        if (!_dc) {
            dismiss();
            return;
        }
        if (_dc(answer)) {
            dismiss();
        }
    }

    void Dialog::dismiss() {
        this->_dialog->runAction(Sequence::create(
            ScaleTo::create(0.15f, 0.1f),
            CallFunc::create(CC_CALLBACK_0(Dialog::onDismiss, this)),
            nullptr));
        __instance = nullptr;;
    }

    void Dialog::onDismiss() {
        this->removeChild(this->_dialog);
        Director::getInstance()->getRunningScene()->removeChild(this);
    }

    bool Dialog::onTouchBegan(Touch * touch, Event * event) {
        return true;
    }

    void Dialog::onTouchMoved(Touch * touch, Event * event) {
    }

    void Dialog::onTouchEnded(Touch * touch, Event * event) {
    }

    void Dialog::onTouchCancelled(Touch * touch, Event * event) {
    }
    
	//inline Dialog* Dialog::setTitle(const char * title) {
	//	this->_title = LabelTTF::create(title, "Arial", 32);
	//	this->_title->setColor(__clrtxt);
	//	return this;
	//}

	//inline Dialog* Dialog::setText(const char * text) {
	//	this->_text = LabelTTF::create(text, "Arial", 24);
	//	this->_text->setColor(__clrtxt);
	//	return this;
	//}

	//inline Dialog* Dialog::setPositive(const char * yes) {
	//	
	//}
}




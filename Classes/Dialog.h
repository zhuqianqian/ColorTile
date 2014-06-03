#ifndef _299SUTDIO_DIALOG_H_
#define _299STUDIO_DIALOG_H_

#include "cocos2d.h"

namespace z299studio {
	typedef std::function<bool(int)> DialogCallback;

	class Dialog : public cocos2d::LayerColor {
	public:

		static cocos2d::MenuItemSprite * createDialogButton(const char *text,
			float width, float height,
			float fntSize, float border,
			const cocos2d::Color3B normal,
			const cocos2d::Color3B pressed,
			const int style,
			const cocos2d::ccMenuCallback &callback);

		CREATE_FUNC(Dialog);
		~Dialog(){}
		static Dialog * build();
		virtual bool init();

		//Dialog * setTitle(const char * title);
		//Dialog * setText(const char * text);
		//Dialog * setPositive(const char * yes);
		//Dialog * setNeutral(const char * neutral);
		//Dialog * setNegative(const char * no);
		//Dialog * setCallback(DialogCallback &callback);
		//Dialog * setOptions(const char *yes, const char *no = nullptr, const char * neutral = nullptr);
		//Dialog * setPositive(const char * yes, cocos2d::ccMenuCallback &cb);
		//Dialog * setNeutral(const char * neutral, cocos2d::ccMenuCallback &cb);
		//Dialog * setNegative(const char * no, cocos2d::ccMenuCallback &cb);
		//void show();
		Dialog* setContentScale(float scale);
		void dismiss();
		void show(const char * title, const char * text, const DialogCallback &callback,
			const char * yes, const char * no = nullptr, const char *neutral = nullptr);

		virtual bool onTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event);
		virtual void onTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event);
		virtual void onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event);
		virtual void onTouchCancelled(cocos2d::Touch * touch, cocos2d::Event * event);

	private:
		void dialogCallback(cocos2d::Ref * pSender, int answer);
        void onDismiss();
		DialogCallback _dc;
		/*cocos2d::LabelTTF * _title,
			*_text,
		    *_yes, 
			*_neutral,
			*_no;*/
		float _scale;
		cocos2d::Sprite * _dialog;
		cocos2d::Menu * _menu;
	};
}

#endif /* _299STUDIO_DIALOG_H_ */

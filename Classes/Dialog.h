#ifndef _299SUTDIO_DIALOG_H_
#define _299STUDIO_DIALOG_H_

#include "cocos2d.h"

namespace z299studio {
	typedef std::function<void(int, void *)> DialogCallback;

	class Dialog : public cocos2d::LayerColor {
	public:
		Dialog() : _dc(nullptr){}
		~Dialog(){}
		static Dialog * build();
		virtual bool init();

		Dialog * setTitle(const char * title);
		Dialog * setText(const char * text);
		Dialog * setPositive(const char * yes);
		Dialog * setNeutral(const char * neutral);
		Dialog * setNegative(const char * no);
		Dialog * setCallback(DialogCallback &callback);
		Dialog * setPositive(const char * yes, cocos2d::ccMenuCallback &cb);
		Dialog * setNeutral(const char * neutral, cocos2d::ccMenuCallback &cb);
		Dialog * setNegative(const char * no, cocos2d::ccMenuCallback &cb);
		void dismiss();
		void show();
		void show(const char * text, DialogCallback & callback, const char * yes,
			const char * no = nullptr, const char *neutral = nullptr);

	private:
		DialogCallback _dc;
		void dialogCallback(cocos2d::Ref * pSender, int answer);
	};
}

#endif /* _299STUDIO_DIALOG_H_ */

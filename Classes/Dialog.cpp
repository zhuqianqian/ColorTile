#include "Dialog.h"

USING_NS_CC;

namespace z299studio {
	
	Dialog* Dialog::build() {
		auto layer = Dialog::create();
		return dynamic_cast<Dialog *> (layer);
	}

	bool Dialog::init() {
		return LayerColor::initWithColor(Color4B(0, 0, 0, 128));
	}
}




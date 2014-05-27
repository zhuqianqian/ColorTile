#ifndef _SHAREDPREF_H_
#define _SHAREDPREF_H_

#include "cocos2d.h"
USING_NS_CC;

class SharedPref {
public:
	static CCUserDefault* ud;
};

typedef SharedPref SP;

#endif /* _SHAREDPREF_H_ */
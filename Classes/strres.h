#ifndef _STRRES_H_
#define _STRRES_H_

#include "cocos2d.h"

enum RSTR {
	new_game = 0,
	resume_game,
	score,
	swipes,
	best,
	next_prev,
	game_over,
	new_best,
	back,
	retry,
	share,
	leaderboard,
	monocolor,
	tutorial0,
	tuturial1,
	tuturial2,
	overwrite_game,
	yes,
	no,
	giveup_ask,
	giveup_yes,
	giveup_no,
	use_google_ask,
	signin,
	_not_used
};

typedef char * cstr;

class StrRes {
private:
	char *_buffer;
	cstr *_ptrs;

public:
	StrRes() :_buffer(NULL), _ptrs(NULL) {}
	~StrRes() {
		delete _buffer;
		delete _ptrs;
	}
	int loadStrings(char *path = nullptr, int lang = -1);
	char* getString(int id);
	static StrRes * getInstance();
	static void killInstance(StrRes* instance = NULL);
};

#endif /* _STRRES_H_ */
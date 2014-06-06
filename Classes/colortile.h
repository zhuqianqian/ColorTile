#ifndef _COLORTILE_H_
#define _COLORTILE_H_

#include "cocos2d.h"

#define COLOR_0	(Color3B(21, 106, 235))
#define COLOR_1 (Color3B(116, 227, 70))
#define COLOR_2 (Color3B(246, 94, 59))
#define COLOR_3 (Color3B(255, 187, 32))
#define COLOR_4 (Color3B(64, 0, 64))
#define COLOR_5 (Color3B(0, 255, 255))
#define COLOR_6 (Color3B(255, 255, 255))

#define SP_HS_CLASSIC	("ClassicHighScore")
#define SP_HS_MONOCOLOR	("MonoColorBest")
#define SP_GAME_STATUS	("GameStatus")
#define SP_GAME_MODE	("GameMode")
#define SP_GAME_SCORE	("Score")
#define SP_GAME_DATA	("GameData")
#define SP_CAND_COLORS	("Candidates")
#define SP_X_COUNT	("SizeX")
#define SP_Y_COUNT	("SizeY")
#define SP_ENABLE_GOOGLE	("UseGoogleGames")
#define SP_ASK_GOOGLE	    ("AskToUseGoogle")
#define SP_TUTORIAL_CLASSIC	("ClassicTutorial")
#define SP_TUTORIAL_MONO	("MonoColorTutorial")
#define SP_SOUND    ("Sound")

#define GAME_NORMAL	(0)
#define GAME_MONOCLR	(1)
#define GAME_RESUME	(-1)

#define GAME_PAUSED	(1)
#define GAME_RUNNING	(0)
#define GAME_OVER	(2)

#define DESIGN_WIDTH (480.0f)

#define TAG_GAMESCENE	(299)

//#define SID_BTN	(0)
//#define SID_TILE (1)
//#define SID_CHEER (2)

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) 
#define SOUND_BTN	("sound/button.wav")
#define SOUND_TILE	("sound/tile.wav")
#define SOUND_CHEER	("sound/cheer.wav")
#define SOUND_REG   ("sound/reg.wav")
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define SOUND_BTN	("sound/button.ogg")
#define SOUND_TILE	("sound/tile.ogg")
#define SOUND_CHEER	("sound/cheer.ogg")
#define SOUND_REG   ("sound/reg.ogg")
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define SOUND_BTN	("sound/button.caf")
#define SOUND_TILE	("sound/tile.caf")
#define SOUND_CHEER	("sound/cheer.caf")
#define SOUND_REG   ("sound/reg.caf")
#else
#define SOUND_BTN	("sound/button.mp3")
#define SOUND_TILE	("sound/tile.mp3")
#define SOUND_CHEER	("sound/cheer.mp3")
#define SOUND_REG   ("sound/reg.mp3")
#endif

#endif /* _COLORTILE_H_ */
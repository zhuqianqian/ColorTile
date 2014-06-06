
#ifndef _GPGHELPER_H_
#define _GPGHELPER_H_

#if defined __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void beginUserInitiatedSigned();
extern void submitScore(int id, int score);
extern void showLeaderboard(int id);
extern void showAllLeaderboards();
extern int isSignedIn();
extern void doShare(int mode, int score);
extern void doRate();
extern void doMoreGames();

#if defined __cplusplus
}
#endif /* __cplusplus */


#endif /* _GPGHELPER_H_ */
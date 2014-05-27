#include "gpghelper.h"

#if defined (ANDROID)
#include "platform/android/jni/JniHelper.h"

#define APPACTIVITY ("org.cocos2dx.cpp/AppActivity")
USING_NS_CC;

extern "C" {
	void beginUserInitiatedSigned() {
		JniMethodInfo t;
		if (JniHelper::getStaticMethodInfo(t, APPACTIVITY,
			"gameServicesSignIn", "()V")) {
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
		}
	}

	void showAllLeaderboards() {
        JniMethodInfo t;
        if( JniHelper::getStaticMethodInfo(t,APPACTIVITY,
            "showAllLeaderboards", "()V")) {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
        }
	}

    void showLeaderboard(int id) {
        JniMethodInfo t;
        if( JniHelper::getStaticMethodInfo(t,APPACTIVITY,
            "showLeaderboard", "(I)V")) {
            t.env->CallStaticVoidMethod(t.classID, t.methodID, id);
        }
	}

    void submitScore(int id, int score) {
        JniMethodInfo t;
        if( JniHelper::getStaticMethodInfo(t,APPACTIVITY,
            "submitScore", "(II)V")) {
            t.env->CallStaticVoidMethod(t.classID, t.methodID, id, score);
        }
	}

    int isSignedIn() {
        JniMethodInfo t;
        if( JniHelper::getStaticMethodInfo(t,APPACTIVITY,
            "gpgsSignedIn", "()I")) {
            return t.env->CallStaticIntMethod(t.classID, t.methodID);
        }
        return 0;
	}

    void doShare(int mode, int score) {
        JniMethodInfo t;
        if( JniHelper::getStaticMethodInfo(t,APPACTIVITY,
            "onShare", "(II)V")) {
            t.env->CallStaticIntMethod(t.classID, t.methodID, mode, score);
        }
	}

}
#elif defined (CC_TARGET_OS_IPHONE)


extern "C" {
    void beginUserInitiatedSigned(){}
    void submitScore(int id, int score){}
    void showLeaderboard(int id){}
    void showAllLeaderboards(){}
    int isSignedIn() { return 0;}
}
#endif /* ANDROID */
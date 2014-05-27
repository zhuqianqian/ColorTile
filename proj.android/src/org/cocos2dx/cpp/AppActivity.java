/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import org.cocos2dx.lib.Cocos2dxActivity;

import com.google.android.gms.games.Games;

import com.z299studio.colortile.BaseGameActivity;
import com.z299studio.colortile.R;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.content.Intent;

public class AppActivity extends BaseGameActivity {
	
    public static Context mContext;
    private static SharedPreferences mSP;
    private boolean gpgs;
    private static String[] lbids;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mContext = AppActivity.this;
        mSP = this.getSharedPreferences("UserDefault", 0);
        gpgs = mSP.getBoolean("UseGoogleGames", false);
        mHelper.setConnectOnStart(gpgs);
        lbids = new String[2];
        lbids[0] = getResources().getString(R.string.leaderboard_classic);
        lbids[1] = getResources().getString(R.string.leaderboard_mono_color);
    }

    @Override
	public void onSignInFailed() {		
	}

	@Override
	public void onSignInSucceeded() {
        if(!gpgs) {
            gpgs = true;
            mSP.edit().putBoolean("UseGoogleGames", gpgs).commit();
        }
	}
	
	public static void onShare(final int mode, final int score) {
		((AppActivity)mContext).runOnUiThread(new Runnable() {
            public void run() {
        		Intent sendIntent = new Intent();
        		sendIntent.setAction(Intent.ACTION_SEND);
                if(mode == 0) {
        		    sendIntent.putExtra(Intent.EXTRA_TEXT, ((AppActivity)mContext).getResources().getString(R.string.share_content0, score));
                }
                else {
                    sendIntent.putExtra(Intent.EXTRA_TEXT, ((AppActivity)mContext).getResources().getString(R.string.share_content1, score));
                }
        		sendIntent.setType("text/plain");
        		((AppActivity)mContext).startActivity(Intent.createChooser(sendIntent, 
        				((AppActivity)mContext).getResources().getString(R.string.send_to)));
           }
        });
	}

    public static void gameServicesSignIn() {
        ((AppActivity)mContext).runOnUiThread(new Runnable() {
            public void run() {
               ((AppActivity)mContext).beginUserInitiatedSignIn();
           }
        });
    }
    
    public static void submitScore(final int id, final int score) {
		Games.Leaderboards.submitScore(((AppActivity)mContext).getApiClient(),
		    lbids[id], score);
    }

    public static void showAllLeaderboards() {
        ((AppActivity)mContext).runOnUiThread(new Runnable() {
            public void run() {
            	((AppActivity)mContext).startActivityForResult(
            		Games.Leaderboards.getAllLeaderboardsIntent(((AppActivity)mContext).getApiClient()),
            		299);
            }
        });
    }
    
    public static void showLeaderboard(final int id) {
        ((AppActivity)mContext).runOnUiThread(new Runnable() {
            public void run() {
                ((AppActivity)mContext).startActivityForResult(
                    Games.Leaderboards.getLeaderboardIntent(((AppActivity)mContext).getApiClient(), lbids[id]),
                    299);
            }
        });
         
    }

    public static int gpgsSignedIn() {
        boolean signedIn = ((AppActivity)mContext).isSignedIn();
        if(signedIn) {
            return 1;
        }
        return 0;
    }
    
//    public static void updateAchievement(String id, int percentage) {
//
//        ((appname)mContext).getGamesClient().incrementAchievement(id,percentage);
//    }
//
//    public static void showAchievements() {
//        ((appname)mContext).runOnUiThread(new Runnable() {
//            public void run() {
//                ((appname)mContext).startActivityForResult(((appname)mContext).getGamesClient().getAchievementsIntent(), 5001);
//            }
//        });
//    }
}

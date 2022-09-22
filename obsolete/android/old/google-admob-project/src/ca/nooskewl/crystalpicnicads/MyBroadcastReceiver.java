package ca.nooskewl.crystalpicnicads;

import android.content.Context;
import android.content.Intent;
import android.content.BroadcastReceiver;
import android.util.Log;

public class MyBroadcastReceiver extends BroadcastReceiver
{
	native static void pauseSound();
	native static void resumeSound();

	public void onReceive(Context context, Intent intent)
	{
		if (intent.getAction() == "android.intent.action.DREAMING_STARTED") {
			Log.d("CrystalPicnic", "Dream started");
			pauseSound();
		}
		else if (intent.getAction() == "android.intent.action.DREAMING_STOPPED") {
			Log.d("CrystalPicnic", "Dream stopped");
			resumeSound();
		}
	}
}

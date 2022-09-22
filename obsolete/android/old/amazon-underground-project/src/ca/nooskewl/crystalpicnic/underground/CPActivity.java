package ca.nooskewl.crystalpicnic.underground;

import org.liballeg.android.AllegroActivity;
import android.net.Uri;
import android.content.Intent;
import android.text.ClipboardManager;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import java.io.File;
import android.util.Log;
import android.app.ActivityManager;
import android.os.Bundle;
import org.json.*;
import java.security.*;
import java.io.*;
import javax.crypto.*;
import javax.crypto.spec.*;
import android.util.*;
import java.util.*;
import java.security.spec.*;
import android.app.Activity;
import android.view.View;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.content.IntentFilter;
import com.amazon.ags.api.*;
import com.amazon.ags.api.achievements.*;
import com.amazon.ags.api.overlay.PopUpLocation;

public class CPActivity extends AllegroActivity {

	/* load libs */
	static {
		System.loadLibrary("allegro_monolith");
		System.loadLibrary("bass");
		System.loadLibrary("bassmidi");
		System.loadLibrary("crystalpicnic");
	}

	int initialize_success = -1;

	//reference to the agsClient
	AmazonGamesClient agsClient;

	AmazonGamesCallback callback = new AmazonGamesCallback() {
		@Override
		public void onServiceNotReady(AmazonGamesStatus status) {
			//unable to use service
			initialize_success = 0;
		}
		@Override
		public void onServiceReady(AmazonGamesClient amazonGamesClient) {
			agsClient = amazonGamesClient;
			//ready to use GameCircle
			agsClient.setPopUpLocation(PopUpLocation.TOP_CENTER);
			initialize_success = 1;
		}
	};

	//list of features your game uses (in this example, achievements and leaderboards)
	EnumSet<AmazonGamesFeature> myGameFeatures = EnumSet.of(AmazonGamesFeature.Achievements);

	public CPActivity()
	{
		super("libcrystalpicnic.so");
	}

	public void logString(String s)
	{
		Log.d("CrystalPicnic", s);
	}

	public String getSDCardPrivateDir()
	{
		File f = getExternalFilesDir(null);
		if (f != null) {
			return f.getAbsolutePath();
		}
		else {
			return getFilesDir().getAbsolutePath();
		}
	}

	public boolean wifiConnected()
	{
		ConnectivityManager connManager = (ConnectivityManager)getSystemService(CONNECTIVITY_SERVICE);
		NetworkInfo mWifi = connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);

		return mWifi.isConnected();
	}

	public void openURL(String url)
	{
		Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse("http://www.crystalpicnic.com"));
		startActivity(intent);
	}

	private boolean clip_thread_done = false;

	public void setClipData(String saveState)
	{
		final String ss = saveState;

		Runnable runnable = new Runnable() {
			public void run() {
				ClipboardManager m = 
					(ClipboardManager)getSystemService(Context.CLIPBOARD_SERVICE);

				m.setText(ss);

				clip_thread_done = true;
				}
			};
		runOnUiThread(runnable);

		while (!clip_thread_done)
			;
		clip_thread_done = false;
	}

	private String clipdata;

	public String getClipData()
	{
		Runnable runnable = new Runnable() {
			public void run() {
				ClipboardManager m = 
					(ClipboardManager)getSystemService(Context.CLIPBOARD_SERVICE);

				clipdata = m.getText().toString();

				clip_thread_done = true;
			}
		};
		runOnUiThread(runnable);

		while (!clip_thread_done)
			;
		clip_thread_done = false;

		return clipdata;
	}
	
	MyBroadcastReceiver bcr;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
    	
		bcr = new MyBroadcastReceiver();
	}
	
	public void onResume() {
		super.onResume();

		registerReceiver(bcr, new IntentFilter("android.intent.action.DREAMING_STARTED"));
		registerReceiver(bcr, new IntentFilter("android.intent.action.DREAMING_STOPPED"));

		initialize_success = -1;

		AmazonGamesClient.initialize(this, callback, myGameFeatures);
	}

	public void onPause() {
		super.onPause();

		unregisterReceiver(bcr);

		if (agsClient != null) {
			agsClient.release();
		}
	}
	
	public boolean gamepadAlwaysConnected()
	{
		return getPackageManager().hasSystemFeature("android.hardware.touchscreen") == false;
	}

	public void unlock_achievement(String id) {
		if (agsClient != null) {
			AchievementsClient acClient = agsClient.getAchievementsClient();
			if (acClient != null) {
				AGResponseHandle<UpdateProgressResponse> handle = acClient.updateProgress(id, 100.0f);
			}
		}
	}

	public void show_achievements()
	{
		if (agsClient != null) {
			AchievementsClient acClient = agsClient.getAchievementsClient();
			if (acClient != null) {
				acClient.showAchievementsOverlay();
			}
		}
	}

	public int initialized()
	{
		return initialize_success;
	}

	public String get_android_language()
	{
		return Locale.getDefault().getLanguage();
	}
}


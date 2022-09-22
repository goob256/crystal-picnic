package com.nooskewl.cp;

import org.liballeg.android.AllegroActivity;
import android.net.Uri;
import android.content.Intent;
import android.text.ClipboardManager;
import android.content.Context;
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
import android.view.ViewGroup;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.content.IntentFilter;

import com.nooskewl.cp.License_Viewer_Activity;

public class CPActivity extends AllegroActivity {
	final static int LICENSE_REQUEST = 1002;

	/* load libs */
	static {
		System.loadLibrary("c++_shared");
		System.loadLibrary("allegro_monolith");
		System.loadLibrary("bass");
		System.loadLibrary("bassmidi");
		System.loadLibrary("crystalpicnic");
	}

	MyBroadcastReceiver bcr;

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

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
    	
		bcr = new MyBroadcastReceiver();
	}

	public void onResume() {
		super.onResume();

		registerReceiver(bcr, new IntentFilter("android.intent.action.DREAMING_STARTED"));
		registerReceiver(bcr, new IntentFilter("android.intent.action.DREAMING_STOPPED"));
	}

	public void onPause() {
		super.onPause();

		unregisterReceiver(bcr);
	}
	
	public boolean gamepadAlwaysConnected()
	{
		return getPackageManager().hasSystemFeature("android.hardware.touchscreen") == false;
	}

	public void onStart() {
		super.onStart();
	}

	public void onStop() {
		super.onStop();
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == LICENSE_REQUEST) {
			if (data != null) {
				if (resultCode == RESULT_OK && data.getExtras().getString("MESSAGE").equals("OK")) {
					show_license_result = 0;
				}
				else if (resultCode == RESULT_CANCELED && data.getExtras().getString("MESSAGE").equals("FAIL")) {
					show_license_result = 1;
				}
				else {
					show_license_result = 1;
				}
			}
			else {
				show_license_result = 1;
			}
		}
	}

	public String get_android_language()
	{
		return Locale.getDefault().getLanguage();
	}
	
	static int show_license_result;

	public void showLicense()
	{
		show_license_result = -1;
		Intent intent = new Intent(this, License_Viewer_Activity.class);
		startActivityForResult(intent, LICENSE_REQUEST);
	}

	public int getShowLicenseResult()
	{
		return show_license_result;
	}
}

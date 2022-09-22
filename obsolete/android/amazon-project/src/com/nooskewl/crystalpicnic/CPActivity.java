package com.nooskewl.crystalpicnic;

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
import com.amazon.device.iap.PurchasingService;
import com.amazon.device.iap.PurchasingListener;
import com.amazon.device.iap.model.PurchaseResponse;
import com.amazon.device.iap.model.PurchaseUpdatesResponse;
import com.amazon.device.iap.model.ProductDataResponse;
import com.amazon.device.iap.model.UserDataResponse;
import com.amazon.device.iap.model.Receipt;
import com.amazon.device.iap.model.FulfillmentResult;
import com.amazon.device.iap.model.RequestId;
import com.amazon.ags.api.*;
import com.amazon.ags.api.achievements.*;
import com.amazon.ags.api.overlay.PopUpLocation;

import com.nooskewl.crystalpicnic.License_Viewer_Activity;

public class CPActivity extends AllegroActivity implements PurchasingListener {
	final static int LICENSE_REQUEST = 1002;

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
			Log.d("CrystalPicnic", "GameCircle not initialized: " + status.toString());
			initialize_success = 0;
		}
		@Override
		public void onServiceReady(AmazonGamesClient amazonGamesClient) {
			Log.d("CrystalPicnic", "GameCircle initialized!");
			agsClient = amazonGamesClient;
			//ready to use GameCircle
			agsClient.setPopUpLocation(PopUpLocation.TOP_CENTER);
			initialize_success = 1;
		}
	};

	// list of features your game uses (in this example, achievements and leaderboards)
	EnumSet<AmazonGamesFeature> myGameFeatures = EnumSet.of(AmazonGamesFeature.Achievements);

	public CPActivity()
	{
		super("libcrystalpicnic.so");
	}

	public void logString(String s)
	{
		Log.d("CrystalPicnic", s);
	}

	MyBroadcastReceiver bcr;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
    	
		bcr = new MyBroadcastReceiver();

		PurchasingService.registerListener(this.getApplicationContext(), this);
		Log.d("CrystalPicnic", "IS_SANDBOX_MODE:" + PurchasingService.IS_SANDBOX_MODE);
	}
	
	public void onResume() {
		super.onResume();

		registerReceiver(bcr, new IntentFilter("android.intent.action.DREAMING_STARTED"));
		registerReceiver(bcr, new IntentFilter("android.intent.action.DREAMING_STOPPED"));

		PurchasingService.getUserData();
		
		initialize_success = -1;

		AmazonGamesClient.initialize(this, callback, myGameFeatures);
	}

	public void onPurchaseUpdatesResponse(final PurchaseUpdatesResponse response)
	{
		// Process receipts
		switch (response.getRequestStatus()) {
			case SUCCESSFUL:
				java.util.List receipts = response.getReceipts();
				boolean found = false;
				for (int i = 0; i < receipts.size(); i++) {
					Receipt r = (Receipt)receipts.get(i);
					if (r.getSku().equals("UNLOCK")) {
						found = true;
						break;
					}
				}
				if (found) {
					Log.d("CrystalPicnic", "purchased=1");
					purchased = 1;
				}
				else {
					Log.d("CrystalPicnic", "purchased=0");
					purchased = 0;
				}
				break;
			default:
				Log.d("CrystalPicnic", "purchased=0 (2)");
				purchased = 0;
				break;
		}
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

	private int purchased = -1;

	public void onPurchaseResponse(final PurchaseResponse response) {
		switch (response.getRequestStatus()) {
			case SUCCESSFUL:
				final Receipt receipt = response.getReceipt();
				if (receipt.getSku().equals("UNLOCK")) {
					PurchasingService.notifyFulfillment(receipt.getReceiptId(), FulfillmentResult.FULFILLED);
					purchased = 1;
					Log.d("Crystal Picnic", "Purchased success!");
				}
				else {
					purchased = 0;
					Log.d("Crystal Picnic", "Purchase failed!");
				}
				break;
			default:
				Log.d("Crystal Picnic", "Purchase failed!");
				purchased = 0;
				break;
		}
	}

	public void doIAP()
	{
		purchased = -1;
		final RequestId requestId = PurchasingService.purchase("UNLOCK");
	}

	public int isPurchased()
	{
		return purchased;
	}

	public void queryPurchased()
	{
		purchased = -1;
		PurchasingService.getPurchaseUpdates(true);
	}

	public void onProductDataResponse(ProductDataResponse productDataResponse)
	{
	}

	public void onUserDataResponse(UserDataResponse userDataResponse)
	{
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
}


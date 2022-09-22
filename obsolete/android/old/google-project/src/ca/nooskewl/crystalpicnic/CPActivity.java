package ca.nooskewl.crystalpicnic;

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
import android.view.ViewGroup;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.content.IntentFilter;

import android.app.Dialog;
import android.support.v4.app.DialogFragment;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.GoogleApiClient.ConnectionCallbacks;
import com.google.android.gms.common.api.GoogleApiClient.OnConnectionFailedListener;
import com.google.android.gms.plus.Plus;
import com.google.android.gms.games.Games;
import android.os.Handler;
import android.os.Looper;
import android.content.IntentSender.SendIntentException;
import android.content.DialogInterface;

import ca.nooskewl.crystalpicnic.License_Viewer_Activity;

public class CPActivity extends AllegroActivity implements ConnectionCallbacks, OnConnectionFailedListener {
	final static int LICENSE_REQUEST = 1002;

	/* load libs */
	static {
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
		
		mResolvingError = savedInstanceState != null && savedInstanceState.getBoolean(STATE_RESOLVING_ERROR, false);
	}

	public void onResume() {
		super.onResume();

		registerReceiver(bcr, new IntentFilter("android.intent.action.DREAMING_STARTED"));
		registerReceiver(bcr, new IntentFilter("android.intent.action.DREAMING_STOPPED"));

		play_services_available = check_play_services_status();
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

		if (play_services_available) {
			api_client.connect();
		}
	}

	public void onStop() {
		super.onStop();

		if (play_services_available) {
			api_client.disconnect();
		}
	}

	boolean play_services_available;
	GoogleApiClient api_client;
	// Request code to use when launching the resolution activity
	private static final int REQUEST_RESOLVE_ERROR = 1001;
	// Unique tag for the error dialog fragment
	private static final String DIALOG_ERROR = "dialog_error";
	// Bool to track whether the app is already resolving an error
	private boolean mResolvingError = false;

	public void onConnectionFailed(ConnectionResult result) {
		play_services_available = false;
		if (mResolvingError) {
			// Already attempting to resolve an error.
			return;
		}
		else if (result.hasResolution()) {
			try {
				mResolvingError = true;
				result.startResolutionForResult(this, REQUEST_RESOLVE_ERROR);
			}
			catch (SendIntentException e) {
				// There was an error with the resolution intent. Try again.
				api_client.connect();
			}
		}
		else {
			// Show dialog using GooglePlayServicesUtil.getErrorDialog()
			showErrorDialog(result.getErrorCode());
			mResolvingError = true;
		}
	}

	/* Creates a dialog for an error message */
	private void showErrorDialog(int errorCode) {
		// Create a fragment for the error dialog
		ErrorDialogFragment dialogFragment = new ErrorDialogFragment();
		// Pass the error that should be displayed
		Bundle args = new Bundle();
		args.putInt(DIALOG_ERROR, errorCode);
		dialogFragment.setArguments(args);
		dialogFragment.show(getSupportFragmentManager(), "errordialog");
	}
	
	/* Called from ErrorDialogFragment when the dialog is dismissed. */
	public void onDialogDismissed() {
		mResolvingError = false;
	}

	/* A fragment to display an error dialog */
	public static class ErrorDialogFragment extends DialogFragment {
		public ErrorDialogFragment() { }

		@Override
		public Dialog onCreateDialog(Bundle savedInstanceState) {
			// Get the error code and retrieve the appropriate dialog
			int errorCode = this.getArguments().getInt(DIALOG_ERROR);
			//return GoogleApiAvailability.getInstance().getErrorDialog(this.getActivity(), errorCode, REQUEST_RESOLVE_ERROR);
			return GooglePlayServicesUtil.getErrorDialog(errorCode, this.getActivity(), REQUEST_RESOLVE_ERROR);
		}

		@Override
		public void onDismiss(DialogInterface dialog) {
			((CPActivity)getActivity()).onDialogDismissed();
		}
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == REQUEST_RESOLVE_ERROR) {
			mResolvingError = false;
			if (resultCode == RESULT_OK) {
				// Make sure the app is not already connected or attempting to connect
				if (!api_client.isConnecting() && !api_client.isConnected()) {
					api_client.connect();
				}
			}
		}
		else if (requestCode == LICENSE_REQUEST) {
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

	private static final String STATE_RESOLVING_ERROR = "resolving_error";

	@Override
	public void onSaveInstanceState(Bundle outState) {
		super.onSaveInstanceState(outState);
		outState.putBoolean(STATE_RESOLVING_ERROR, mResolvingError);
	}

	private boolean check_play_services_status() {
		int status = GooglePlayServicesUtil.isGooglePlayServicesAvailable(getApplicationContext());

		if (status == ConnectionResult.SUCCESS) {
			return true;
		}
		else {
			Dialog d = GooglePlayServicesUtil.getErrorDialog(status, this, 0);
			d.show();
			return false;
		}
	}

	public void onConnected(Bundle connectionHint) {
		play_services_available = true;
		if (play_services_available && api_client.isConnected()) {
			Games.setViewForPopups(api_client, findViewById(android.R.id.content));
		}
	}

	public void onConnectionSuspended(int cause) {
	}

	public void init_play_services() {
		play_services_available = check_play_services_status();

		if (play_services_available) {
			api_client = new GoogleApiClient.Builder(getApplicationContext())
				.addConnectionCallbacks(this)
				.addOnConnectionFailedListener(this)
				.addApi(Games.API)
				.addScope(Games.SCOPE_GAMES)
				.build();

			api_client.connect();
		}
	}
	
	public void unlock_achievement(String id) {
		if (play_services_available) {
			logString("Achieving " + id);
			try {
				Games.Achievements.unlock(api_client, id);
			}
			catch (Exception e) {
			}
		}
		else {
			logString("Play Services not available, can't achieve :(");
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

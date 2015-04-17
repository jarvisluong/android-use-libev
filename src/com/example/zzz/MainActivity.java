package com.example.zzz;

import android.app.ActionBar;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class MainActivity extends Activity {
	String tag = "MainActivity";
	Button btn;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// requestWindowFeature(Window.FEATURE_NO_TITLE);
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		ActionBar actionBar = getActionBar();
		actionBar.setDisplayOptions(ActionBar.DISPLAY_SHOW_HOME
				| ActionBar.DISPLAY_SHOW_TITLE | ActionBar.DISPLAY_SHOW_CUSTOM);

		btn = (Button) findViewById(R.id.button1);
		btn.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

			}
		});
		
		Intent intent = new Intent();
		intent.setClass(MainActivity.this, NetService.class);
		startService(intent);
	}

	@Override
	protected void onDestroy() {
		Intent intent = new Intent();
		intent.setClass(MainActivity.this, NetService.class);
		stopService(intent);
		super.onDestroy();
	}
}

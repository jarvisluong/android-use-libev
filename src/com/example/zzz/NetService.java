package com.example.zzz;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

public class NetService extends Service {

	public static final String tag = "NetService";
	
	@Override
	public void onCreate() {
		Log.i(tag, "onCreate");
		server_init_native();
		super.onCreate();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void onDestroy() {
		Log.i(tag, "onDestroy");
		server_exit_native();
		super.onDestroy();
	}

	public void onEvent(int code, String msg) {
		Log.i(tag, "onEvent");
	}
	
	static {
		Log.i(tag, android.os.Build.CPU_ABI);
		String CPU_ABI = android.os.Build.CPU_ABI;
		if (CPU_ABI.contains("x86")) {
			System.loadLibrary("ev-x86");
		} else {
			System.loadLibrary("ev-arm");
		}
		System.loadLibrary("hello");
	}
	public native int server_init_native();
	public native int server_exit_native();
	public native int send_msg_native(String msg);
}

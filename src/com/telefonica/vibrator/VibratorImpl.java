package com.telefonica.vibrator;


import android.app.Activity;
import android.os.Vibrator;

public class VibratorImpl {
	private VibratorNative _native;
	private Vibrator vir;
	
	public void cancel(int req) {
		_native.cancel(req);
	}
	
	public void cancelAll() {
		_native.cancelAll();
	}
	
	/*
	public int connect() {
		// return vibe.OpenDevice(0, "DH75NDX26JR75GCZ5CKKTD25M9JP62TG");
	} */
	
	public int simple(int duration,Object options) {
		return _native.simple(duration,options);
		// vir.vibrate(duration);
	}
	
	public int multiple(int[] pattern,int repeatTimes) {
		return _native.multiple(pattern,repeatTimes);
	}
	
	public VibratorImpl(Vibrator vir) {
		_native = new VibratorNative();
		this.vir = vir;
	}
}
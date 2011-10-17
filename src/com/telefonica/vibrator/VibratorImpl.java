package com.telefonica.vibrator;

import com.immersion.android.ImmVibe;

import android.app.Activity;
import android.os.Vibrator;

public class VibratorImpl {
	private VibratorNative _native;
	private Vibrator vir;
	private ImmVibe vibe;
	
	public void cancel(int req) {
		_native.cancel(req);
	}
	
	/*
	public int connect() {
		// return vibe.OpenDevice(0, "DH75NDX26JR75GCZ5CKKTD25M9JP62TG");
	} */
	
	public int simple(int duration) {
		return _native.simple(duration);
		// vir.vibrate(duration);
	}
	
	public int multiple(int[] pattern,int repeatTimes) {
		return _native.multiple(pattern,repeatTimes);
	}
	
	public VibratorImpl(Vibrator vir) {
		_native = new VibratorNative();
		this.vir = vir;
		this.vibe = new ImmVibe();
	}
}
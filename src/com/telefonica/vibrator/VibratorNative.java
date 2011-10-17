package com.telefonica.vibrator;

public class VibratorNative {
	public native int simple(int duration);
	public native int multiple(int[] pattern,int repeatTimes);
	
	public native void cancel(int reqId);
}

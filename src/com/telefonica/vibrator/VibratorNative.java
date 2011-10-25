package com.telefonica.vibrator;

public class VibratorNative {
	public native int simple(int duration,Object options);
	public native int multiple(int[] pattern,int repeatTimes);
	
	public native void cancel(int reqId);
	
	public native void cancelAll();
}

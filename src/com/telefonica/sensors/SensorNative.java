package com.telefonica.sensors;

import android.util.Log;

public class SensorNative {
	public native int getMinDelay(int handle);
	
	public native String connect(String uri);
	
	public native void watch(int handle,int interval);
	
	public native void end(int handle);
	
	public native void kill(int handle);
	
	public native void killAll();
	
	public native String listSensors();
}
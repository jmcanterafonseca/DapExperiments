package com.telefonica.sensors;

import android.util.Log;

public class SensorNative {
	public native String getType(int handle);
	
	public native String getVendor(int handle);
	
	public native String getName(int handle);
	
	public native int getMinDelay(int handle);
	
	public native float getResolution(int handle);
	
	public native int connect(String uri);
	
	public native void watch(int handle,int interval);
	
	public native void end(int handle);
	
	public native void kill(int handle);
	
	public native void killAll();
}
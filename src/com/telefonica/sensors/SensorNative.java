package com.telefonica.sensors;

public class SensorNative {
	public native String getType(int handle);
	
	public native String getVendor(int handle);
	
	public native String getName(int handle);
	
	public native int getMinDelay(int handle);
	
	public native float getResolution(int handle);
	
	public native int connect(String uri);
	
	public native void watch(int handle,long interval);
	
	public native void end(int handle);
}
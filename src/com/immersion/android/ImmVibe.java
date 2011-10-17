package com.immersion.android;

public class ImmVibe {
	public native int OpenDevice(int number,String s);
	public native void setDevicePropertyInt32(int handle, int number, int value);
}
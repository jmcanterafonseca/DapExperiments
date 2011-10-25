package com.telefonica.sensors;

import android.util.Log;
import android.webkit.WebView;

public class SensorImpl {
	
	private SensorNative _native;
	private static WebView wv;

	public String getVendor(int handle) {
		// TODO Auto-generated method stub
		return _native.getVendor(handle);
	}

	public String getType(int handle) {
		// TODO Auto-generated method stub
		return _native.getType(handle);
	}
	
	public String getName(int handle) {
		// TODO Auto-generated method stub
		return _native.getName(handle);
	}
	
	public int getMinDelay(int handle) {
		// TODO Auto-generated method stub
		return _native.getMinDelay(handle);
	}
	
	public float getResolution(int handle) {
		// TODO Auto-generated method stub
		return _native.getResolution(handle);
	}
	

	public void watch(int handle,int delay) {
		_native.watch(handle, delay);
	}
	
	public void end(int handle) {
		_native.end(handle);
	}
	
	public void kill(int handle) {
		_native.kill(handle);
	}
	
	public void killAll() {
		_native.killAll();
	}
	
	public int connect(String uri) {
		return _native.connect(uri);
	}
	
	// Hello 
	public SensorImpl(WebView v) {
		_native = new SensorNative();
		SensorImpl.wv = v;
	}
	
	public static void sensorDataCB(int handle,String event) {
		
		Log.d("sensors-java","Event received!!!!!" + event);
		
		
		wv.loadUrl("javascript:_sensorDataCB(" + handle + "," + event + ")");
	}
	
	public void init() {
		// _native.setEventTarget(this);
	}
}

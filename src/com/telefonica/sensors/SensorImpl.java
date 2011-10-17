package com.telefonica.sensors;

public class SensorImpl {
	
	private SensorNative _native;

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
	
	public int connect(String str) {
		return _native.connect(str);
	}

	// Hello 
	public SensorImpl() {
		_native = new SensorNative();
	}
}

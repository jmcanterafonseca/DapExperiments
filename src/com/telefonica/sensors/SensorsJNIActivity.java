package com.telefonica.sensors;

import org.w3c.sensors.R;

import com.telefonica.vibrator.VibratorImpl;

import android.app.Activity;
import android.os.Bundle;
import android.os.Vibrator;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.widget.ImageView;

public class SensorsJNIActivity extends Activity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

		setContentView(R.layout.main2);

		WebView appView = (WebView) findViewById(R.id.appView);
		appView.setWebChromeClient(new MyWebChromeClient());
		appView.getSettings().setJavaScriptEnabled(true);

		SensorImpl impl = new SensorImpl(appView);
		impl.init();
		
		appView.addJavascriptInterface(impl, "_internal_sensor");
		appView.addJavascriptInterface(new VibratorImpl((Vibrator)getSystemService(VIBRATOR_SERVICE)), "_internal_vibrator");

		appView.loadUrl("file:///android_asset/webcontent/index.html");
    }
    
    static {
    	System.loadLibrary("native-sensors");
    	// System.loadLibrary("ImmVibeJ");
    }
    
    final class MyWebChromeClient extends WebChromeClient {
	}
}
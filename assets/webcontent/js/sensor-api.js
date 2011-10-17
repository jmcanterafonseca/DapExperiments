if(!window.navigator.SensorConnection) {
	window.navigator.SensorConnection =  function(sensorURI) {
		var suri = sensorURI;
		var connectionHandle;
		
		connectionHandle = _internal_sensor.connect(suri);
		
		this.startWatch = function(interval) {
			_internal_sensor.watch(interval);
			// return uri;
		}
		
		this.endWatch = function() {
			_internal_sensor.end();
		}
		
		this.sensor = new function() {		
			this.type = (function() { return _internal_sensor.getType(connectionHandle) })();
			this.name = (function() { return _internal_sensor.getName(connectionHandle) })();
			this.vendor = (function() { return _internal_sensor.getVendor(connectionHandle) })();
			this.minDelay = (function() { return _internal_sensor.getMinDelay(connectionHandle) })();
			this.resolution = (function() { return _internal_sensor.getResolution(connectionHandle) })();
		}
	}
}

if(!window.ConnectionFactory) {
}

function sensorData(handle,data) {
	
}
var connections = {};

if(!window.navigator.SensorConnection) {
	window.navigator.SensorConnection =  function(sensorURI) {
		var suri = sensorURI;
		var connectionHandle;
		var that = this;
		
		connectionHandle = _internal_sensor.connect(suri);
		
		connections[connectionHandle.toString()] = this;
		
		function setStatus(val) {
			that.status = val;
			if(that.onstatuschange) {
				that.onstatuschange();
			}
		}
		
		this.startWatch = function(interval) {
			_internal_sensor.watch(connectionHandle,interval);
			setStatus("watching");
		}
		
		this.endWatch = function() {
			_internal_sensor.end(connectionHandle);
			setStatus("open");
		}
		
		this.sensor = new function() {		
			this.type = (function() { return _internal_sensor.getType(connectionHandle) })();
			this.name = (function() { return _internal_sensor.getName(connectionHandle) })();
			this.vendor = (function() { return _internal_sensor.getVendor(connectionHandle) })();
			this.minDelay = (function() { return _internal_sensor.getMinDelay(connectionHandle) })();
			this.resolution = (function() { return _internal_sensor.getResolution(connectionHandle) })();
		}
		
		setStatus("open");
	}
}

if(!window.navigator.sensors) {
	window.navigator.sensors = new function() {
		this.killAll = function() {
			_internal_sensor.killAll();
		}
	}
}


// Invoked when new sensor data is available at the native layer
function _sensorDataCB(handle,data) {
	window.console.log("Handle Received: " + handle);
	
	// Obtain the connection object and then invoke the onsensordata handler
	var connection = connections[handle.toString()];
	
	if(connection.onsensordata) {
		connection.onsensordata(data);
	}
}
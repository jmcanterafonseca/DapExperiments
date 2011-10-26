var connections = {};

if(!window.navigator.SensorConnection) {
	window.navigator.SensorConnection =  function(sensorURI) {
		var suri = sensorURI;
		var connectionHandle;
		var that = this;
			
		var metadata = eval('(' + _internal_sensor.connect(sensorURI) + ')');
		connectionHandle = metadata.handle;
		
		connections[connectionHandle.toString()] = this;
		
		this.sensor = metadata.sensor;
		
		function setStatus(val) {
			that.status = val;
			if(that.onstatuschange) {
				that.onstatuschange();
			}
		}
		
		this.startWatch = function(options) {
			if(this.status == "connected") {
				_internal_sensor.watch(connectionHandle,options.interval);
				setStatus("watching");
			}
		}
		
		this.endWatch = function() {
			if(this.status == "watching") {
				_internal_sensor.end(connectionHandle);
				setStatus("connected");
			}
		}
		
		setStatus("connected");
	}
}

if(!window.navigator.sensors) {
	window.navigator.sensors = new function() {
		this.killAll = function() {
			_internal_sensor.killAll();
		}
		
		this.list = function() {
			window.console.log("Listed");
			return eval(_internal_sensor.list());
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
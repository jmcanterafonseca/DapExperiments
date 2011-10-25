if(!window.navigator.service) {
	window.navigator.service = new function() {
		this.vibrator = new function() {
			this.play = function(time,options) {
				return (_internal_vibrator.simple(time,options));
			}
			this.mplay = function(pattern,repeat) {
				return (_internal_vibrator.multiple(pattern,repeat));
			}		
			this.cancel = function(requestId) {
				_internal_vibrator.cancel(requestId);
			}
			
			this.cancelAll = function() {
				_internal_vibrator.cancelAll();
			}
			
			this.connect = function() {
				_internal_vibrator.connect();
			}
		}
	} 
}
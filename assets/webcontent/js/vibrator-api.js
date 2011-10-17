if(!window.navigator.service) {
	window.navigator.service = new function() {
		this.vibrator = new function() {
			this.simple = function(time) {
				return (_internal_vibrator.simple(time));
			}
			this.multiple = function(pattern,repeat) {
				return (_internal_vibrator.multiple(pattern,repeat));
			}		
			this.cancel = function(requestId) {
				_internal_vibrator.cancel(requestId);
			}
			this.connect = function() {
				_internal_vibrator.connect();
			}
		}
	} 
}
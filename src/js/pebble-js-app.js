var deviceId = "";
var accessToken = "";

/*
 * Import jQuery
 */
var importjQuery = function() {
	var script = document.createElement('script');
	script.src = 'http://code.jquery.com/jquery-latest.min.js';
	script.type = 'text/javascript';
	document.getElementsByTagName('head')[0].appendChild(script);
};

/**************** Pebble helpers ****************/
var hasKey = function(dict, key) {
	return dict.payload[key] != undefined;
};

var getValue = function(dict, key) {
	if(hasKey(dict, key)) {
		return "" + dict.payload[key];
	} else {
		console.log("ERROR: Key '" + key + "' does not exist in received dictionary");
		return undefined;
	}
};

/************************************************/

Pebble.addEventListener("ready",
	function(e) {
		importjQuery();
		console.log("Pebble JS ready!");
	}
);

var updatePebble = function(json) {
	console.log("return_value=" + json.return_value);
	
	Pebble.sendAppMessage(
		{"PIN_EVENT":json.return_value},
		function(e) {
			console.log("Send response to Pebble");
		},
		function(e) {
			console.log("FAILED to send response to Pebble");
		}
	);
};

var success = function(json) {
	console.log("Response received: " + JSON.stringify(json));
	updatePebble(json);
};

Pebble.addEventListener("appmessage",
	function(dict) {
		//On request?
		if(hasKey(dict, "PIN_ON")) {
			console.log("Received turn on pin: " + getValue(dict, "PIN_ON"));
			
			//Construct URL
			var url = "https://api.spark.io/v1/devices/" + deviceId + "/on?access_token=" + accessToken;

			console.log("jQuery AJAX args=" + getValue(dict, "PIN_ON"));

			//Send with jQuery
			$.ajax({
			  type: "POST",
			  url: url,
			  data: {"args":getValue(dict, "PIN_ON")},
			  success: success,
			  dataType: "json"
			});
		} 
		
		//Off request?
		if(hasKey(dict, "PIN_OFF")) {
			console.log("Received turn off pin: " + getValue(dict, "PIN_OFF"));
		
			//Construct URL
			var url = "https://api.spark.io/v1/devices/" + deviceId + "/off?access_token=" + accessToken;

			console.log("jQuery AJAX args=" + getValue(dict, "PIN_OFF"));

			//Send with jQuery
			$.ajax({
			  type: "POST",
			  url: url,
			  data: {"args":getValue(dict, "PIN_OFF")},
			  success: success,
			  dataType: "json"
			});
		} 
	}
);

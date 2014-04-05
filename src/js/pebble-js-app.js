var deviceId = "YOURDEVICEID";
var accessToken = "YOURACCESSTOKEN";

var REST = function (url, type, callback) {
	var xhr = new XMLHttpRequest();
	xhr.onload =  function () {
      callback(this.responseText);
	};
	xhr.open(type, url);
	xhr.send();
};

Pebble.addEventListener("ready",
  function(e) {
  	console.log("Pebble JS ready!");
  }
);

var parseCloudResponse = function(text) {
	var json = JSON.parse(text);
	
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

Pebble.addEventListener("appmessage",
	function(e) {
		//On request?
		if(e.payload["PIN_ON"] != undefined) {
			console.log("Received turn on pin: " + e.payload["PIN_ON"]);
			
			//Construct url
			var url = "https://api.spark.io/v1/devices/" + deviceId + "/on?access_token=" + accessToken + "&args=" + e.payload["PIN_ON"];
			console.log("Sent request to: " + url);
			
			//Send
			REST(url, "POST", 
				function(text) {
					console.log("Response received: " + text);
					
					//Parse result
					parseCloudResponse(text);
				}
			);
		} 
		
		//Off request?
		if(e.payload["PIN_OFF"] != undefined) {
			console.log("Received turn off pin: " + e.payload["PIN_OFF"]);
			
			//Construct url
			var url = "https://api.spark.io/v1/devices/" + deviceId + "/off?access_token=" + accessToken + "&args=" + e.payload["PIN_OFF"];
			console.log("Sent request to: " + url);
			
			//Send
			REST(url, "POST", 
				function(text) {
					console.log("Response received: " + text);
					
					//Parse result
					parseCloudResponse(text);
				}
			);
		}
	}
);

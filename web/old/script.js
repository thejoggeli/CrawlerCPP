$(document).ready(function(){
	$(".inp-btn").each(function(){
		$(this).data("pressed", false);
	});
	$(".inp-btn").on("mousedown touchstart", function(){
		MatrixServer.sendInput(0, $(this).data("keycode"));
		$(this).data("pressed", true);
	});
	$(".inp-btn").on("mouseup mouseout touchend", function(){
		if($(this).data("pressed")){
			$(this).data("pressed", false)
			MatrixServer.sendInput(1, $(this).data("keycode"));
		}
	});
	$(".msg-btn").on("click", function(){
		MatrixServer.sendMessage($(this).data("msg"));
	});
	$(".cmd-btn").on("click", function(){
		MatrixServer.sendCommand($(this).data("cmd"), $(this).data("params"));
	});
	$(".enter-fullscreen").on("click", function(){
		Fullscreen.enter();
	});
	$(".leave-fullscreen").on("click", function(){
		Fullscreen.leave();
	});
	$(".toggle-fullscreen").on("click", function(){
		Fullscreen.toggle();
	});
	$(document).on("fullscreenchange mozfullscreenchange webkitfullscreenchange msfullscreenchange", function(){
		Fullscreen.onChange();
	});
	Fullscreen.onChange();
	MatrixServer.connect();
});

/*
function setConnectedMode(mode){
	if(mode){
		$("button").prop("disabled", false);
		$(".connected-state").text("connected");
		$(".connected-state").addClass("connected");
		$(".connected-state").removeClass("disconnected");
	} else {
		$("button").prop("disabled", true);
		$(".connected-state").text("disconnected");
		$(".connected-state").addClass("disconnected");
		$(".connected-state").removeClass("connected");
	}
	updateWindowTitle();
}
*/

function updateWindowTitle(){
	var connectedStr = connected ? "connected" : "disconnected";
	document.title = "Raspi [" + connectedStr + "]";
}

function MatrixServer(){}
MatrixServer.eventListeners = [];
MatrixServer.websocket = null;
MatrixServer.connect = function(){
	var hostname = window.location.hostname;
	var url = "ws://"+hostname+":9090/ws";
	var ws = MatrixServer.websocket = new WebSocket(url);
	console.log("attempting to connect to " + url + " ...");
	ws = new WebSocket(url);
	ws.onopen = function(evt){
		console.log("connection opened");
		for(var e in MatrixServer.eventListeners){
			if(MatrixServer.eventListeners[e].onConnectionOpen !== undefined){
				MatrixServer.eventListeners[e].onConnectionOpen();
			}
			if(MatrixServer.eventListeners[e].onConnectionChange !== undefined){
				MatrixServer.eventListeners[e].onConnectionChange();
			}
		}
	};
	ws.onmessage = function(evt){
		console.log("onmessage: " + evt.data);
	};
	ws.onclose = function(evt){
		console.log("connection closed");
		// fire close event if websocket was open
		if(MatrixServer.websocket.readyState == WebSocket.CLOSING){
			MatrixServer.websocket = null;
			for(var e in MatrixServer.eventListeners){
				if(MatrixServer.eventListeners[e].onConnectionClose !== undefined){
					MatrixServer.eventListeners[e].onConnectionClose();
				}
			}
		}
		// retry connection
		setTimeout(function(){
			MatrixServer.connect();
		}, 1000);
	};
	ws.onerror = function(evt){
		console.log("onerror: " + evt.data);
	};
}
MatrixServer.addEventListener = function(listener){
	MatrixServer.eventListeners.push(listener);
}
MatrixServer.isConnected = function(){
	var ws = MatrixServer.websocket;
	return ws !== null && ws !== undefined && ws.readyState === WebSocket.OPEN;
}
MatrixServer.send = function(str){
	console.log("Sending: " + str);
	if(MatrixServer.isConnected()){
		MatrixServer.websocket.send(str);
	}
}
MatrixServer.sendInput = function(state, key){
	MatrixServer.send("inp" + state + "" + key);

}
MatrixServer.sendMessage = function(msg){
	MatrixServer.send("msg" + msg);
}
MatrixServer.sendCommand = function(command, params){
	var str = "cmd" + command + ":";
	if(!(params instanceof Array)){
		params = [params];
	}
	for(var p in params){
		var pstr = params[p] + "";
		str += pstr.length + "/" + pstr;
	}
	MatrixServer.send(str);
}

function Fullscreen(){}
Fullscreen.updateButtons = function(){
	if(Fullscreen.isEnabled()){
		// show leave fullscrenn
		$(".enter-fullscreen").hide();
		$(".leave-fullscreen").show();
	} else {
		// show enter fullscreen
		$(".enter-fullscreen").show();
		$(".leave-fullscreen").hide();	
	}
}
Fullscreen.isEnabled = function(){
	var doc = window.document;
	var docEl = doc.documentElement;
	if(doc.fullscreenElement || doc.mozFullScreenElement || doc.webkitFullscreenElement || doc.msFullscreenElement) {
		return true;
	}
	return false;
};
Fullscreen.enter = function(){
	var doc = window.document;
	var docEl = doc.documentElement;
	var enter = docEl.requestFullscreen || docEl.mozRequestFullScreen || docEl.webkitRequestFullScreen || docEl.msRequestFullscreen;
	enter.call(docEl);
}
Fullscreen.leave = function(){
	var doc = window.document;
	var docEl = doc.documentElement;
	var leave = doc.exitFullscreen || doc.mozCancelFullScreen || doc.webkitExitFullscreen || doc.msExitFullscreen;
	leave.call(doc);
}
Fullscreen.toggle = function(){
	if(Fullscreen.isEnabled()){
		Fullscreen.leave();
	} else {
		Fullscreen.enter();
	}
}
Fullscreen.onChange = function(){
	Fullscreen.updateButtons();
}


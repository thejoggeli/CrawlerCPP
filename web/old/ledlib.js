function MatrixClient(){}
MatrixClient.debugLog = false;
MatrixClient.eventListeners = [];
MatrixClient.websocket = null;
MatrixClient.connectedFlag = false;
MatrixClient.connect = function(){
	var hostname = window.location.hostname;
	var url = "ws://"+hostname+":9090/ws";
	var ws = MatrixClient.websocket = new WebSocket(url);
	console.log("attempting to connect to " + url + " ...");
	ws.onopen = function(event){
		console.log("connection opened");
		MatrixClient.connectedFlag = true;
		for(var e in MatrixClient.eventListeners){
			if(MatrixClient.eventListeners[e].onWebsocketOpen !== undefined){
				MatrixClient.eventListeners[e].onWebsocketOpen();
			}
			if(MatrixClient.eventListeners[e].onWebsocketChange !== undefined){
				MatrixClient.eventListeners[e].onWebsocketChange();
			}
		}
	};
	ws.onmessage = function(event){
		var message = $.parseJSON(event.data);
		console.log("Message from server: " + message.type);
		console.log(message);
		for(var e in MatrixClient.eventListeners){
			if(MatrixClient.eventListeners[e].onWebsocketMessage !== undefined){
				MatrixClient.eventListeners[e].onWebsocketMessage(message);
			}
		}
	};
	ws.onclose = function(event){
		console.log("connection closed");
		// fire close event if websocket was open
		console.log(MatrixClient.websocket.readyState);
		if(MatrixClient.connectedFlag){
			MatrixClient.connectedFlag = false;
			MatrixClient.websocket = null;
			for(var e in MatrixClient.eventListeners){
				if(MatrixClient.eventListeners[e].onWebsocketClose !== undefined){
					MatrixClient.eventListeners[e].onWebsocketClose();
				}
				if(MatrixClient.eventListeners[e].onWebsocketChange !== undefined){
					MatrixClient.eventListeners[e].onWebsocketChange();
				}
			}
		}
		// retry connection
		setTimeout(function(){
			MatrixClient.connect();
		}, 1000);
	};
	ws.onerror = function(event){
		console.log("onerror: " + event.data);
	};
}
MatrixClient.addEventListener = function(listener){
	MatrixClient.eventListeners.push(listener);
}
MatrixClient.removeEventListener = function(listener){
	var array = MatrixClient.eventListeners;
	for (var i = array.length-1; i >= 0; i--) {
	if (array[i] === listener) {
		array.splice(i, 1);
	}
}
}
MatrixClient.isConnected = function(){
	var ws = MatrixClient.websocket;
	return ws !== null && ws !== undefined && ws.readyState === WebSocket.OPEN;
}
MatrixClient.send = function(str){
	if(MatrixClient.debugLog) console.log("Sending: " + str);
	if(MatrixClient.isConnected()){
		MatrixClient.websocket.send(str);
	}
}
MatrixClient.sendMessage = function(msg, params){
	if(params === null || params === undefined){
		MatrixClient.send("msg"+msg);		
	} else {
		var str = "msg" + msg + ":";
		if(!(params instanceof Array)){
			params = [params];
		}
		for(var p in params){
			var pstr = params[p] + "";
			str += pstr.length + "/" + pstr;
		}
		MatrixClient.send(str);		
	}
}
MatrixClient.sendInput = function(state, code){
	MatrixClient.send("inp" + state + "" + code);

}
MatrixClient.sendJoystick = function(state, code, rel_x, rel_y){
	if(state != 2){
		MatrixClient.send("joy"+state+""+code);
	} else {
		var sign_x = rel_x >= 0 ? "+" : "";
		var sign_y = rel_y >= 0 ? "+" : "";
		MatrixClient.send("joy"+state+""+code+"/"+sign_x+rel_x+"/"+sign_y+rel_y);
	}
}

function Sfx(){}
Sfx.files = {};
Sfx.currentMusicFile = null;
Sfx.onWebsocketMessage = function(json){
	if(json.type == "load_sfx"){
		if(!(json.files instanceof Array)){
			json.files = [json.files];
		}
		if(!(json.volume instanceof Array)){
			json.volume = [json.volume];
		}
		if(!(json.start instanceof Array)){
			json.start = [json.start];
		}
		if(!(json.end instanceof Array)){
			json.end = [json.end];
		}
		for(var x in json.files){
			Sfx.loadFile(json.files[x], json.start[x], json.end[x], json.volume[x]);
		}
	} else if(json.type == "play_sound"){
		Sfx.playSound(json);
	} else if(json.type == "start_music"){
		Sfx.startMusic(json);
	} else if(json.type == "stop_music"){
		Sfx.stopMusic();
	}
}
Sfx.onWebsocketClose = function(){
	Sfx.stopMusic();
}
Sfx.playSound = function(json){
	var file = Sfx.getFile(json.file);
	if(file != null){
		file.volume = json.volume !== undefined ? json.volume : file._def_volume;
		file.currentTime = json.start !== undefined ? json.start : file._def_start;
		file.loop = false;
		file.play();
	}
	if(Sfx.currentMusicFile != null && Sfx.currentMusicFile.currentTime == 0){
		Sfx.currentMusicFile.play();
	}
}
Sfx.startMusic = function(json){
	var file = Sfx.getFile(json.file);
	if(file != null){
		if(Sfx.currentMusicFile == file){
			console.log("Music already playing");			
		} else {
			if(Sfx.currentMusicFile != null){
				Sfx.stopMusic();
			}
			Sfx.currentMusicFile = file;
			file.volume = json.volume !== undefined ? json.volume : file._def_volume;
			file.currentTime = json.start !== undefined ? json.start : file._def_start;
			file.loop = true;
			file.play();
		}
	}	
}
Sfx.stopMusic = function(){
	if(Sfx.currentMusicFile != null){
		Sfx.currentMusicFile.pause();
		Sfx.currentMusicFile = null;
	}	
}
Sfx.getFile = function(name){
	if(Sfx.files[name] === undefined){
		console.log("Audio file not found: " + name);
		return null;
	}
	return Sfx.files[name];
}
Sfx.loadFile = function(name, start, end, volume){
	var audio;
	if(Sfx.files[name] !== undefined){
		console.log("Audio file already added: " + name);
		audio = Sfx.files[name];
	} else {
		audio = new Audio();
		audio.src = "sfx/" + name;
		Sfx.files[name] = audio;
		console.log("loading: " + audio.src);
		audio.addEventListener("canplaythrough", function(){
			console.log("ready: " + this.src);
		});		
	}
	audio._def_start = start;
	audio._def_end = end;
	audio._def_volume = volume;
}

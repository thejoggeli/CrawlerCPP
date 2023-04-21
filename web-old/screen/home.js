function Home(){}
Home.adminUnlocked = false;
Home.status = null 
Home.init = function(){
	$(document).on("click", "#home .app-btn", function(){
		$(this).addClass("semi-active");
		AppManager.startApp($(this).data("app"));
	});
	var $apps = $("#home .apps");
	for(var key in AppManager.apps){
		var handle = AppManager.apps[key];
		var name = handle.name;
		var $row = $("<div class='app-row'>");
		var $btn = $("<button class='app-btn' data-app='"+key+"'>");
		$btn.html(name);
		$row.append($btn);
		$apps.append($row);
	}
	$("#home .admin-row").hide()
	$("#home .admin-unlock-row").on("click", Home.adminPrompt)
	MatrixClient.addEventListener(Home);
}
Home.adminPrompt = function(){
	var val = ""+prompt("PASSWORD");
	if(val == "420"){
		Home.adminUnlocked = true;
		$("#home .admin-unlock-row").hide();
		$("#home .admin-row").show();
	} else {
		alert("NEIN!!!");
	}
}
Home.open = function(){
	Home.updateBrightness(AppManager.brightness);
	Home.updateConnectedMode();
	Home.updateActiveApp();
	Home.updateStatusInfo()
}
Home.close = function(){
	
}
Home.updateActiveApp = function(){
	$("#home .app-row").removeClass("active");
	$("#home .app-btn").removeClass("active");
	$("#home .app-row").removeClass("semi-active");
	$("#home .app-btn").removeClass("semi-active");
	var activeApp = AppManager.activeApp;
	if(activeApp == null) return;
	var $btn = $("#home .app-btn[data-app='"+activeApp+"']");
	$btn.addClass("active");
	$btn.closest("#home .app-row").addClass("active");
	console.log($btn);
}
Home.updateStatusInfo = function(){
	if(!MatrixClient.isConnected()){
		$("#home .status-row .status-value").html("&ndash;")
		$("#home .status-row .status-unit").html("&ndash;")
		
	} else {
		if(Home.status == null){
			var $loader = $("<img>");
			$loader.attr("src", "svg/ajax-loader-black.svg")
			$loader.attr("width", 12) 
			$loader.attr("height", 12) 
			$("#home .status-row .status-value").empty().append($loader.clone())
			$("#home .status-row .status-unit").empty().append($loader.clone())
		} else {			
			var json = Home.status
			var fps = json.fps
			var clients = json.clients
			var temp = roundToFixed(json.temp, 1)
			var clock = parseInt(hzToSize(json.clock).split(" "))*1000
			var mem_total = bytesToSize(json.mem_total).split(" ")
			var mem_used = bytesToSize(json.mem_used).split(" ")
			$("#home .status-row .status-fps .status-value").text(fps)
			$("#home .status-row .status-fps .status-unit").html("")
			$("#home .status-row .status-clients .status-value").text(clients)
			$("#home .status-row .status-clients .status-unit").html("")
			$("#home .status-row .status-temp .status-value").text(temp)
			$("#home .status-row .status-temp .status-unit").text("°C")
			$("#home .status-row .status-clock .status-value").text($.trim(clock[0]))
			$("#home .status-row .status-clock .status-unit").text($.trim(clock[1]))
			$("#home .status-row .status-mem-total .status-value").text($.trim(mem_total[0]))
			$("#home .status-row .status-mem-total .status-unit").text($.trim(mem_total[1]))
			$("#home .status-row .status-mem-used .status-value").text($.trim(mem_used[0])) 
			$("#home .status-row .status-mem-used .status-unit").text($.trim(mem_used[1])) 			
		}
	}
	
}
Home.updateConnectedMode = function(){
	var mode = MatrixClient.isConnected();
	if(mode){
		$("#home button").prop("disabled", false);
		$("#home .connected-state").text("Online");
		$("#home .connected-state").addClass("connected");
		$("#home .connected-state").removeClass("disconnected");
	} else {
		$("#home button").prop("disabled", true);
		$("#home .connected-state").text("Offline");
		$("#home .connected-state").addClass("disconnected");
		$("#home .connected-state").removeClass("connected");
	}
}
Home.onWebsocketOpen = function(){
	Home.updateStatusInfo()	
}
Home.onWebsocketClose = function(){
	$("#home .app-row").removeClass("active");
	$("#home .app-btn").removeClass("active");
	$("#home .app-row").removeClass("semi-active");
	$("#home .app-btn").removeClass("semi-active");	
	Home.status = null
	Home.updateStatusInfo()
}
Home.onWebsocketChange = function(){
	Home.updateConnectedMode();	
}
Home.onWebsocketMessage = function(json){
	if(json.type == "welcome"){
		Home.updateBrightness(json.brightness);
		Home.updateActiveApp();
	} else if(json.type == "set_brightness"){
		Home.updateBrightness(json.brightness);
	} else if(json.type == "status"){
		Home.status = json;
		Home.updateStatusInfo()
	}
}
Home.updateBrightness = function(val){
//	$("#home .msg-btn[data-msg=set_brightness]").removeClass("selected");
//	$("#home .msg-btn[data-msg=set_brightness][data-params="+val+"]").addClass("selected");
	$("#home .brightness-slider").val(val);
	$("#home .brightness-label .value").text(val);
}


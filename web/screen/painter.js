function Painter(){}
Painter.pixels;
Painter.repaintRequired = true
Painter.init = function(){
	Painter.width = 64;
	Painter.height = 32;
	Painter.aspect = Painter.width/Painter.height;
	Painter.pixels = new Array(Painter.width);
	for(var x = 0; x < Painter.width; x++){
		Painter.pixels[x] = new Array(Painter.height);
		for(var y = 0; y < Painter.height; y++){
			Painter.pixels[x][y] = {};
			Painter.setPixelColor(x, y, 0, 0, 0);
		}
	}
	PainterPenTouch.init();
	PainterToolsMenu.init();
	PainterTools.init();
	$("#painter .tool-button").on("click", function(){
		PainterTools.selectTool($(this).data("tool"));		
	});
	$("#painter .range-container").on("touchmove touchstart touchend", function(e){
		if($(e.target).hasClass("range-container")){
			e.preventDefault();
			e.stopPropagation();
		}
	});
			
//	Painter.$rose = $("#painter .rose");
//	Painter.onRoseDragInit();


}
/*
Painter.onRoseDragInit = function(){
	$(document).on("mousedown touchstart", "#painter .rose", Painter.onRoseDragStart);
}
Painter.onRoseDragStart = function(e){
	console.log("DRAG START");
	$(document).off("mousedown touchstart", "#painter .rose", Painter.onRoseDragStart);
	$(document).on("mousemove touchmove", Painter.onRoseDragMove);
	$(document).on("mouseup touchend", Painter.onRoseDragEnd);
	if(e.originalEvent.touches){
		var touch = e.originalEvent.touches[0];
		var rect = e.currentTarget.getBoundingClientRect();
		Painter.dragOffsetX = touch.clientX - rect.left;
		Painter.dragOffsetY = touch.clientY - rect.top;
	} else {
		var rect = e.currentTarget.getBoundingClientRect();
		Painter.dragOffsetX = e.clientX - rect.left;
		Painter.dragOffsetY = e.clientY - rect.top;		
	}
	e.preventDefault();
}
Painter.onRoseDragMove = function(e){
	if(e.originalEvent.touches){
		var touch = e.originalEvent.touches[0];
		Painter.$rose.css("left", touch.clientX-Painter.dragOffsetX);
		Painter.$rose.css("top", touch.clientY-Painter.dragOffsetY);	
	} else {
		Painter.$rose.css("left", e.clientX-Painter.dragOffsetX);
		Painter.$rose.css("top", e.clientY-Painter.dragOffsetY);			
	}
	e.preventDefault();
}
Painter.onRoseDragEnd = function(e){
	$(document).on("mousedown touchstart", "#painter .rose", Painter.onRoseDragStart);
	$(document).off("mousemove touchmove", Painter.onRoseDragMove);
	$(document).off("mouseup touchend", Painter.onRoseDragEnd);
	e.preventDefault();
} */
Painter.open = function(){
	Haf.install({
		height:100, 
		container:$("#painter .canvas-target"), 
	});	
	Haf.onResize = Painter.resize;
	Haf.onUpdate = Painter.update;
	Haf.onRender = Painter.render;
	Haf.getCanvas(0).setActive();
	Haf.getCanvas(0).clearColor = "black";
	Haf.getCanvas(0).autoClear = false;
	Haf.inputOverlay.on("mousedown touchstart", function(){
		$("#painter .toolbar-overlay").hide();
		if(PainterTools.selectedTool == PainterTools.tools.menu){
			PainterTools.selectTool("pen");
		}
	});
	$("#painter .toolbar-overlay").hide();
	Haf.start();
	Haf.focus();
	MatrixClient.addEventListener(Painter);
	Colors.hslToRgb255(randomFloat(0, 1.0), 1.0, 0.5);
	Painter.introTimerDuration = 5.0;
	Painter.introTimer = Painter.introTimerDuration;
	PainterTools.open();
	Painter.repaintRequired = true;
	MatrixClient.sendMessage("request_pixels");
}
Painter.close = function(){
	MatrixClient.removeEventListener(Painter);
	PainterTools.close();
	Haf.uninstall();
}
Painter.resize = function(){	
	Painter.repaintRequired = true;
}
Painter.update = function(){
	if(Painter.introTimer > 0.0){
		Painter.introTimer -= Time.deltaTime;
		Painter.repaintRequired = true;
	}	
	PainterTools.selectedTool.update();
	Haf.getCanvas(0).autoClear = Painter.repaintRequired;
}
Painter.render = function(){
	if(!Painter.repaintRequired){
		return;
	}
	
	var windowAspect = Haf.width / Haf.height;
	ctx.save();
	var w = Painter.width+2;
	var h = Painter.height+2;
	var a = w/h;
	if(windowAspect > a){
		ctx.scale(Haf.height/h, Haf.height/h);
	} else {
		ctx.scale(Haf.width/w, Haf.width/w);
	}	
	ctx.translate(-Painter.width/2, -Painter.height/2);
	
	ctx.fillStyle = "#333";
	ctx.fillRect(-1, -1, Painter.width+2, Painter.height+2);
	ctx.fillStyle = "black";
	ctx.fillRect(0, 0, Painter.width, Painter.height);
	
	for(var x = 0; x < Painter.width; x++){
		for(var y = 0; y < Painter.height; y++){
			var p = Painter.pixels[x][y];
			ctx.fillStyle = p.str;
			ctx.fillRect(x+0.1, y+0.1, 0.8, 0.8);			
		}
	}
	ctx.restore();
	if(Painter.introTimer > 0.0){
	/*	ctx.save();
		var hue1 = (Time.sinceStart * 0.2)%1.0;
		var hex1 = Colors.hslToHex(hue1, 1.0, 0.75);
		var hue2 = (Time.sinceStart * 0.2)%1.0;
		var hex2 = Colors.hslToHex(hue2, 1.0, 0.25);
		ctx.globalAlpha = Math.sin(Math.PI*Painter.introTimer/Painter.introTimerDuration);
		var scale = Math.sin(Math.PI*Painter.introTimer/Painter.introTimerDuration)*1.5;
		ctx.scale(scale, scale);
		ctx.rotate(Math.sin(Math.PI*Painter.introTimer/Painter.introTimerDuration*2.0)*0.1);
		ctx.fillStyle = hex1;
		ctx.strokeStyle = hex2;
		ctx.textAlign = "center";
		ctx.textBaseline = "middle";
		ctx.font = (Haf.width*0.08)+"px Arial";
		ctx.lineWidth = Haf.width*0.005;
		ctx.strokeText("draw something!", 0, 0);
		ctx.fillText("draw something!", 0, 0);
		ctx.restore(); */
		ctx.save();
		
				
		var alpha = 1.0;
		var scale = 1.0;
		if(Painter.introTimer/Painter.introTimerDuration >= 0.5){
			scale = Math.sin(Math.PI*Painter.introTimer/Painter.introTimerDuration);
		}
		if(Painter.introTimer/Painter.introTimerDuration <= 0.25){
			alpha = Math.sin(Math.PI*Painter.introTimer/Painter.introTimerDuration*2);
			scale = Math.sin(Math.PI*Painter.introTimer/Painter.introTimerDuration*2);
		}
		ctx.globalAlpha = alpha;
		ctx.translate(-3, 0);
		ctx.scale(scale, scale);
		ctx.rotate(Math.sin(Math.PI*Painter.introTimer/Painter.introTimerDuration*2.0)*0.05);
		ctx.translate(Painter.introTimer*2.5, 0);
		ctx.fillStyle = "white";
		ctx.strokeStyle = "black";
		ctx.textAlign = "center";
		ctx.textBaseline = "middle";
		ctx.font = "bold " + (Haf.width*0.08)+"px Courier New";
		ctx.lineWidth = Haf.width*0.005;
		ctx.strokeText("Draw Something!", 0, 0);
		ctx.fillText("Draw Something!", 0, 0);
		ctx.restore();
	}
	Painter.repaintRequired = false;
}
Painter.transform = function(p){
	var windowAspect = Haf.width / Haf.height;
	var w = Painter.width+2;
	var h = Painter.height+2;
	var a = w/h;
	if(windowAspect > a){
		p.x *= h/Haf.height;
		p.y *= h/Haf.height;
	} else {
		p.x *= w/Haf.width;
		p.y *= w/Haf.width;
	}	
	p.x += Painter.width/2;
	p.y += Painter.height/2;
	p.x = Numbers.clamp(p.x, 0, Painter.width-1);
	p.y = Numbers.clamp(p.y, 0, Painter.height-1);
}
Painter.onWebsocketOpen = function(){
	MatrixClient.sendMessage("request_pixels");
}
Painter.onWebsocketMessage = function(json){
	if(json.type == "pixel"){
		var x = parseInt(json.x);
		var y = parseInt(json.y);
		var r = parseInt(json.rgb.charAt(0)+json.rgb.charAt(1), 16);
		var g = parseInt(json.rgb.charAt(2)+json.rgb.charAt(3), 16);
		var b = parseInt(json.rgb.charAt(4)+json.rgb.charAt(5), 16);
		Painter.setPixelColor(x, y, r, g, b);		
		Painter.updateUndoRedo(json.undo, json.redo);
	} else if(json.type == "pixels"){
		var x = 0;
		var y = 0;
		var len = json.rgb.length-1; // h suffix
		for(var i = 0; i < len; i+=6){
			var r = parseInt(json.rgb.charAt(i+0)+json.rgb.charAt(i+1), 16);
			var g = parseInt(json.rgb.charAt(i+2)+json.rgb.charAt(i+3), 16);
			var b = parseInt(json.rgb.charAt(i+4)+json.rgb.charAt(i+5), 16);
			Painter.setPixelColor(x, y, r, g, b);
			x++;
			if(x >= Painter.width){
				x = 0;
				y++;
			}
		}
		Painter.updateUndoRedo(json.undo, json.redo);
	} else if(json.type == "pixels_chunk"){
		var x = json.x;
		var y = json.y;
		var left = json.x;
		var right = left+json.w;
		var len = json.rgb.length-1; // h suffix
		for(var i = 0; i < len; i+=6){
			if(json.rgb.charAt(i) != "-"){
				var r = parseInt(json.rgb.charAt(i+0)+json.rgb.charAt(i+1), 16);
				var g = parseInt(json.rgb.charAt(i+2)+json.rgb.charAt(i+3), 16);
				var b = parseInt(json.rgb.charAt(i+4)+json.rgb.charAt(i+5), 16);
				Painter.setPixelColor(x, y, r, g, b);
			}
			x++;
			if(x >= right){
				x = left;
				y++;
			}
		}
		Painter.updateUndoRedo(json.undo, json.redo);
	}
}
Painter.updateUndoRedo = function(undo, redo){
	if(undo > 0){
		$("#painter .undo").removeClass("disabled");
	} else {
		$("#painter .undo").addClass("disabled");
	}
	if(redo > 0){
		$("#painter .redo").removeClass("disabled");
	} else {
		$("#painter .redo").addClass("disabled");
	}
}
Painter.setPixelColor = function(x, y, r, g, b){
	var p = Painter.pixels[x][y];
	if(p.r255 == r && p.g255 == g && p.b255 == b){
		return false;
	}
	p.r255 = r;
	p.g255 = g;
	p.b255 = b;
	p.str = "rgb(" + r + ", " + g + ", " + b + ")";
	p.rgb = (r<<16)|(g<<8)|b;
	Painter.repaintRequired = true;
	return true;
}

function PainterTools(){}
PainterTools.init = function(){
	$("#painter .close-overlay").on("click", function(){
		$("#painter .toolbar-overlay").hide();		
		if(PainterTools.selectedTool == PainterTools.tools.menu){
			PainterTools.selectTool("pen");
		}
	});
	$("#painter .open-overlay").on("click", function(){
		$("#painter .toolbar-overlay").show();	
	});
	PainterTools.colors = [];
	PainterTools.grayColors = [];
	PainterTools.hueColors = [];
	var lig_steps = 5;
	var lig_step = 1/(lig_steps-1);
	for(var l = 0; l < lig_steps; l++){
		var color = {
			index: l,
			hue: 0,
			saturation: 0,
			lightness: 1-l*lig_step,
			r255: Colors.r255,
			g255: Colors.g255,
			b255: Colors.b255,
			hex: Colors.rgbToHex(Colors.r255, Colors.g255, Colors.b255),
			rgb: (Colors.r255<<16)|(Colors.g255<<8)|Colors.b255,
		};
		PainterTools.colors.push(color);
		PainterTools.grayColors.push(color);
	}
	PainterTools.colorWhite = PainterTools.colors[0];
	PainterTools.colorBlack = PainterTools.colors[lig_steps-1];
			
	var hsls = [
		0, 	 	// red
		30,	 	// red-orange
		60,	 	// yellow
		90,	 	// green yellowish
		180, 	// cyan
		210, 	// cyan-blue
		240, 	// blue
		270, 	// purple
		300,	// magenta
		330,	// magenta-red	
	];
	for(var h = 0; h < hsls.length; h++){
		var color = {
			index: h+lig_steps,
			hue: hsls[h]/360.0,
			saturation: 0,
			lightness: 0,
			r255: 0, 
			g255: 0, 
			b255: 0,
			hex: "#000000",
			rgb: 0,
		};		
		PainterTools.colors.push(color);
		PainterTools.hueColors.push(color);
	}
	
	var $boxes = $("#painter .color-boxes");
	var $row = null;
	for(var i in PainterTools.colors){
		if(i%lig_steps == 0){
			$row = $("<div class='color-box-row'>");
			$boxes.append($row);
		}
		var $box = $("<div class='color-box'>");	
		$box.data("index", i);
		PainterTools.colors[i].$box = $box;	
		$row.append($box);
	}
		
	$($boxes.find(".color-box")[0]).addClass("white");
	$("#painter .color-boxes").on("click", ".color-box", function(){
		PainterTools.selectColor($(this).data("index"));
	});
	$("#painter .sizepicker input[type=range]").on("input", function(){
		PainterTools.selectSize(parseInt($(this).val()));
	});
/*	$("#painter .colorpicker .lightness").on("input", function(){
		PainterTools.setLightness(parseFloat($(this).val()));
	});
	$("#painter .colorpicker .saturation").on("input", function(){
		PainterTools.setSaturation(parseFloat($(this).val()));
	}); */
	$("#painter .colorpicker .lightness-button").on("click", function(){
		PainterTools.setLightness(parseFloat($(this).data("value")));
	});
	PainterTools.selectColor(randomInt(lig_steps, lig_steps+hsls.length));
}
PainterTools.open = function(){	
	PainterTools.selectedSize = 1;
	PainterTools.selectedTool = null;
	PainterTools.setLightness(0.5);
	PainterTools.setSaturation(1.0);
	PainterTools.tools = {
		pen: new PainterToolsPen(),
		bucket: new PainterToolsBucket(),
		eraser: new PainterToolsEraser(),
		menu: new PainterToolsMenu(),
	}
	PainterTools.selectTool("pen");	
}
PainterTools.close = function(){
	PainterTools.selectedTool.onUnselect();
	PainterTools.tools = null;
}
PainterTools.selectTool = function(tool){
	$("#painter .toolbar-label-extra").hide();
	$("#painter .tool-button").removeClass("selected");
	$("#painter .tool-options").hide();
	var button = $("#painter .tool-button[data-tool='"+tool+"']");
	button.addClass("selected");
	if(PainterTools.selectedTool != null){
		PainterTools.selectedTool.onUnselect();
	}	
	PainterTools.selectedTool = PainterTools.tools[tool];
	PainterTools.selectedTool.onSelect();
	$("#painter .toolbar-label-value").text(PainterTools.selectedTool.name);
}
PainterTools.selectColor = function(index){
	$("#painter .color-box").removeClass("selected");
	$($("#painter .color-boxes .color-box")[index]).addClass("selected");
	PainterTools.selectedColor = PainterTools.colors[index];
	PainterTools.updateColorPreview();
}
PainterTools.setLightness = function(lightness){
	for(var i in PainterTools.hueColors){
		PainterTools.hueColors[i].lightness = lightness;
	}
//	$("#painter .colorpicker .lightness").val(lightness);
//	$("#painter .colorpicker .lightness-value").text(Math.round(lightness*100)+"%");
	$("#painter .colorpicker .lightness-button").removeClass("selected");
	$("#painter .colorpicker .lightness-button[data-value='"+roundToFixed(lightness, 1)+"']").addClass("selected");
	PainterTools.updateColors();
	PainterTools.updateColorPreview();
}
PainterTools.setSaturation  = function(saturation){	
	for(var i in PainterTools.hueColors){
		PainterTools.hueColors[i].saturation = saturation;
	}
//	$("#painter .colorpicker .saturation").val(saturation);
//	$("#painter .colorpicker .saturation-value").text(Math.round(saturation*100)+"%");
	PainterTools.updateColors();
	PainterTools.updateColorPreview();
}
PainterTools.updateColors = function(){
	for(var i in PainterTools.colors){
		var color = PainterTools.colors[i];
		var $box = color.$box;
		Colors.hslToRgb255(color.hue, color.saturation, color.lightness);
		color.r255 = Colors.r255;
		color.g255 = Colors.g255;
		color.b255 = Colors.b255;
		color.hex = Colors.rgbToHex(Colors.r255, Colors.g255, Colors.b255);
		color.rgb = (color.r255<<16)|(color.g255<<8)|color.b255
		$box.css("background-color", color.hex);	
	}	
}
PainterTools.updateColorPreview = function(){
	$("#painter .toolbar-label-extra-color").css("background", PainterTools.selectedColor.hex);	
}
PainterTools.selectSize = function(size){
	PainterTools.selectedSize = size;
	$("#painter .sizepicker .size-value").html(size);
	$("#painter .sizepicker input[type=range]").val(size);
	$("#painter .toolbar-label-extra-size").text(size);
}

function PainterToolsPen(){
	this.savedSize = 1;
	this.name = "Draw";
}
PainterToolsPen.prototype.onSelect = function(){
	this.touches = [];
	PainterTools.selectSize(this.savedSize);
	$("#painter .tool-options.colorpicker").show();
	$("#painter .tool-options.sizepicker").show();
	$("#painter .tool-options.sizepicker").show();
	$("#painter .toolbar-label-extra-color").show();
//	$("#painter .toolbar-label-extra-size").show();
}
PainterToolsPen.prototype.onUnselect = function(){
	for(var t in this.touches){
		this.touches[t].canceled = true;
	}
	this.savedSize = PainterTools.selectedSize;
}
PainterToolsPen.prototype.update = function(){
	for(var i in Input.newTouches){		
		var touch = Input.newTouches[i];
		if(touch.taken) continue;
		touch.taken = true;
		this.touches.push(new PainterPenTouch(touch, PainterTools.selectedColor, PainterTools.selectedSize));
	}
	// remove expired touches
	for(var i = this.touches.length-1; i >= 0; i--){
		if (this.touches[i].touch.expired) {
			this.touches.splice(i, 1);
		}
	}
	// update touches
	for(var t in this.touches){
		this.touches[t].update();
	} 
}

function PainterToolsBucket(){
	this.v1 = new Vector();
	this.name = "Fill";
	this.affected_map = new Array(Painter.width);
	for(var i = 0; i < Painter.width; i++){
		this.affected_map[i] = new Array(Painter.height);
	}
}
PainterToolsBucket.prototype.onSelect = function(){
	$("#painter .tool-options.colorpicker").show();	
	$("#painter .toolbar-label-extra-color").show();
}
PainterToolsBucket.prototype.onUnselect = function(){
	
}
PainterToolsBucket.prototype.update = function(){
	
	var touch = false;
	if(Input.newTouches.length > 0){
		touch = Input.newTouches[0];
	}	
		
	if(touch){
		this.v1.x = touch.worldPosition.x;
		this.v1.y = touch.worldPosition.y;
		Painter.transform(this.v1);
		var nx = Math.floor(this.v1.x);
		var ny = Math.floor(this.v1.y);
		var tc = Painter.pixels[nx][ny];
		var rc = PainterTools.selectedColor;
		
		this.bounds_left = Painter.width;
		this.bounds_right = 0;
		this.bounds_top = Painter.height;
		this.bounds_bottom = 0;
		for(var i = 0; i < Painter.width; i++){
			for(var j = 0; j < Painter.height; j++){				
				this.affected_map[i][j] = false;
			}
		}
		
		this.floodFill(nx, ny, tc.rgb, rc.rgb);
						
		var chunk_size = 16;
		var data = null;
		var right = this.bounds_right+1;
		var bottom = this.bounds_bottom+1;
		var firstChunk = true;
		for(var cx = this.bounds_left; cx < right; cx += chunk_size){
			var chunk_w = cx+chunk_size < right ? chunk_size : (right-cx)
			console.log("w", chunk_w);
			for(var cy = this.bounds_top; cy < bottom; cy += chunk_size){
				var chunk_h = cy+chunk_size < bottom ? chunk_size : (bottom-cy)
				var saveBit = firstChunk ? 1 : 0;
				console.log("h", chunk_h);
				data = new Array(chunk_w*chunk_h+5);
				data[0] = saveBit;
				data[1] = cx; // x offset
				data[2] = cy; // y offset
				data[3] = chunk_w; // width
				data[4] = chunk_h; // height
				var i = 5;
				for(var x = 0; x < chunk_w; x++){
					for(var y = 0; y < chunk_h; y++){
						if(this.affected_map[cx+x][cy+y]){
							data[i++] = Painter.pixels[cx+x][cy+y].rgb;
						} else {
							data[i++] = "-";
						}
					}
				}
				MatrixClient.sendMessage("pixels_chunk", data);
				firstChunk = false;
			}
		} 
		/*
		var pixels = new Array(Painter.width*Painter.height);
		var i = 0;
		for(var x = 0; x < Painter.width; x++){
			for(var y = 0; y < Painter.height; y++){
				pixels[i++] = Painter.pixels[x][y].rgb;
			}
		}
		MatrixClient.sendMessage("pixels", pixels);
		*/
	}
}
PainterToolsBucket.prototype.floodFill = function(nx, ny, tc, rc){
	// https://en.wikipedia.org/wiki/Flood_fill#The_algorithm
	var nc = Painter.pixels[nx][ny].rgb;
	if(tc == rc){
		//  1. If target-color is equal to replacement-color, return.
		return;
	} else if(nc != tc){
		// 2. ElseIf the color of node is not equal to target-color, return.
		return;
	} else {
		this.affected_map[nx][ny] = true;		
		if(nx < this.bounds_left)	this.bounds_left = nx;
		if(nx > this.bounds_right)	this.bounds_right = nx;
		if(ny < this.bounds_top)	this.bounds_top = ny;
		if(ny > this.bounds_bottom)	this.bounds_bottom = ny;
		Painter.setPixelColor(nx, ny, (rc>>16)&0xFF, (rc>>8)&0xFF, rc&0xFF);
		if(nx > 0) 					this.floodFill(nx-1, ny, tc, rc);
		if(nx < Painter.width-1) 	this.floodFill(nx+1, ny, tc, rc);
		if(ny > 0) 					this.floodFill(nx, ny-1, tc, rc);
		if(ny < Painter.height-1) 	this.floodFill(nx, ny+1, tc, rc);		
	}
} 

function PainterToolsMenu(){
	this.name = "Menu";
}
PainterToolsMenu.saveTimeout = null;
PainterToolsMenu.saveTimeoutCounter = 0;
PainterToolsMenu.init = function(){
	$("#painter .tool-options.menu .undo").on("click", function(){
		if(!$(this).hasClass("disabled")){
			PainterToolsMenu.undo();
		}
	});
	$("#painter .tool-options.menu .redo").on("click", function(){
		if(!$(this).hasClass("disabled")){
			PainterToolsMenu.redo();
		}
	});
	$("#painter .tool-options.menu .save").on("click", function(){
		if(!$(this).hasClass("disabled")){
			PainterToolsMenu.save();
		}
	});
	$("#painter .tool-options.menu .home").on("click", function(){
		ScreenManager.open("home");
	});
}
PainterToolsMenu.prototype.onSelect = function(){
	$("#painter .tool-options.menu").show();
	var now = (new Date()).getTime();
	PainterToolsMenu.saveTimeoutCounter = Cookie.read("painter_save_counter", now);
	if(PainterToolsMenu.saveTimeoutCounter > now){
		PainterToolsMenu.startSaveTimeout();
	} else {
		$("#painter .save").removeClass("disabled");
		$("#painter .save span").text("Save");		
	}
}
PainterToolsMenu.prototype.onUnselect = function(){
	if(PainterToolsMenu.saveTimeout != null){
		clearInterval(PainterToolsMenu.saveTimeout);		
		PainterToolsMenu.saveTimeout = null;
	}
}
PainterToolsMenu.prototype.update = function(){
}
PainterToolsMenu.save = function(){
	if(PainterToolsMenu.saveTimeout != null) return;
	MatrixClient.sendMessage("save_image");	
	PainterToolsMenu.saveTimeoutCounter = (new Date()).getTime() + 15*1000;
	Cookie.write("painter_save_counter", PainterToolsMenu.saveTimeoutCounter);
	PainterToolsMenu.startSaveTimeout();
}
PainterToolsMenu.startSaveTimeout = function(){
	if(PainterToolsMenu.saveTimeout != null) return;
	$("#painter .save").addClass("disabled");
	var rem = (PainterToolsMenu.saveTimeoutCounter-(new Date()).getTime())/1000.0;
	$("#painter .save span").text("Save (" + roundToFixed(rem, 1) + ")");	
	PainterToolsMenu.saveTimeout = setInterval(function(){
		var now = (new Date()).getTime();
		var remaining = (PainterToolsMenu.saveTimeoutCounter-now)/1000.0;
		if(remaining <= 0){
			$("#painter .save").removeClass("disabled");
			$("#painter .save span").text("Save");
			clearInterval(PainterToolsMenu.saveTimeout)		
			PainterToolsMenu.saveTimeout = null;
		} else {
			$("#painter .save span").text("Save (" + roundToFixed(remaining, 1) + ")");			
		}
	}, 100);	
}
PainterToolsMenu.undo = function(){
	MatrixClient.sendMessage("undo");
}
PainterToolsMenu.redo = function(){
	MatrixClient.sendMessage("redo");
}

function PainterToolsEraser(){
	this.savedSize = 8;
	this.name = "Erase";
}
PainterToolsEraser.prototype.onSelect = function(){
	this.touches = [];
	PainterTools.selectSize(this.savedSize);
	$("#painter .tool-options.sizepicker").show();
//	$("#painter .toolbar-label-extra-size").show();
}
PainterToolsEraser.prototype.onUnselect = function(){
	for(var t in this.touches){
		this.touches[t].canceled = true;
	}	
	this.savedSize = PainterTools.selectedSize;
}
PainterToolsEraser.prototype.update = function(){
	for(var i in Input.newTouches){		
		var touch = Input.newTouches[i];
		if(touch.taken) continue;
		touch.taken = true;
		this.touches.push(new PainterPenTouch(touch, PainterTools.colorBlack, PainterTools.selectedSize));
	}		
	// remove expired touches
	for(var i = this.touches.length-1; i >= 0; i--){
		if (this.touches[i].touch.expired) {
			this.touches.splice(i, 1);
		}
	}
	// update touches
	for(var t in this.touches){
		this.touches[t].update();
	} 	
}

function PainterPenTouch(touch, color, size){
	this.touch = touch;
	this.color = color;
	this.size = size;
	this.p1 = new Vector();
	this.p2 = new Vector();
	this.last = new Vector(this.touch.worldPosition.x, this.touch.worldPosition.y);
	Painter.transform(this.last);
	this.canceled = false;	
	this.affected_map = new Array(Painter.width);
	for(var i = 0; i < Painter.width; i++){
		this.affected_map[i] = new Array(Painter.height);
	}
	this.firstSend = true;
}
PainterPenTouch.init = function(){
	var stamps = PainterPenTouch.stamps = {};
	var max = parseInt($("#painter .sizepicker input[type=range]").attr("max"));
	for(var i = 1; i <= max; i++){
		var radius = i/2.0;
		var stamp = new Array(i);
		var radius_sqr = (radius-0.1)*(radius-0.1);
		var off = -radius+0.5;
		for(var x = 0; x < i; x++){
			stamp[x] = new Array(i);
			var xo = x+off;
			for(var y = 0; y < i; y++){
				var yo = y+off;
				var dist_sqr = xo*xo+yo*yo;
				stamp[x][y] = dist_sqr < radius_sqr ? 1 : 0;
			}
		}
		stamps[i] = stamp;
	}
}
PainterPenTouch.prototype.applyStamp = function(x, y){
	var stamp = PainterPenTouch.stamps[this.size];
	var x_left = x-Math.floor(this.size/2);
	var y_top = y-Math.floor(this.size/2);	
	for(var i = 0; i < this.size; i++){
		var xx = x_left+i;
		if(xx < 0 || xx >= Painter.width) continue;
		for(var j = 0; j < this.size; j++){
			var yy = y_top+j;
			if(yy < 0 || yy >= Painter.height) continue;
			if(stamp[i][j] != 0){
				if(Painter.setPixelColor(xx, yy, this.color.r255, this.color.g255, this.color.b255)){
					this.affected_map[xx][yy] = true;
					if(xx < this.bounds_left)	this.bounds_left = xx;
					if(xx > this.bounds_right)	this.bounds_right = xx;
					if(yy < this.bounds_top)	this.bounds_top = yy;
					if(yy > this.bounds_bottom)	this.bounds_bottom = yy;
				}
			}
		}
	}	
}
PainterPenTouch.prototype.update = function(){
	if(this.touch.expired || this.canceled){
		console.log("touch expired");
		return;
	}
	
	var p1 = this.p1;
	var p2 = this.p2;
	var p3 = this.p3;
	
	p1.x = this.touch.worldPosition.x;
	p1.y = this.touch.worldPosition.y;
	
	Painter.transform(p1);
	
	var color = this.color;
	
	p2.x = this.last.x;
	p2.y = this.last.y;
	
	var step = 0.25*this.size;
	var dx = p1.x-p2.x;
	var dy = p1.y-p2.y;
	var dist = Math.sqrt(dx*dx+dy*dy);
			
	this.bounds_left = Painter.width;
	this.bounds_right = 0;
	this.bounds_top = Painter.height;
	this.bounds_bottom = 0;
	if(this.size > 1){	
		for(var i = 0; i < Painter.width; i++){
			for(var j = 0; j < Painter.height; j++){				
				this.affected_map[i][j] = false;
			}
		}
	}
	
	if(dist == 0){	
		this.applyStamp(Math.floor(p1.x), Math.floor(p1.y));
	} else {
		var dirx = (p2.x-p1.x)/dist;
		var diry = (p2.y-p1.y)/dist;
		for(var s = 0; s <= dist; s+=step){
			this.applyStamp(Math.floor(p1.x + dirx * s), Math.floor(p1.y + diry * s));
		}
	}
	
	this.bounds_width = this.bounds_right-this.bounds_left+1;
	this.bounds_height = this.bounds_bottom-this.bounds_top+1;
	
	var saveBit = this.firstSend ? 1 : 0;
	
	if(this.bounds_width == 1 && this.bounds_height == 1){
		MatrixClient.sendMessage("pixel", [
			saveBit, this.color.rgb, this.bounds_left, this.bounds_top,
		]);		
		this.firstSend = false;
	} else if(this.bounds_width > 0 && this.bounds_height > 0){
		var data = new Array(this.bounds_width*this.bounds_height+5);
		var k = 5;
		data[0] = saveBit;
		data[1] = this.bounds_left;
		data[2] = this.bounds_top;
		data[3] = this.bounds_width;
		data[4] = this.bounds_height;
		for(var i = this.bounds_left; i <= this.bounds_right; i++){
			for(var j = this.bounds_top; j <= this.bounds_bottom; j++){
				if(this.affected_map[i][j]){
					data[k++] = Painter.pixels[i][j].rgb;
				} else {
					data[k++] = "-";
				}
			}
		}
		MatrixClient.sendMessage("pixels_chunk", data);
		this.firstSend = false;
	}
	this.last.x = p1.x;
	this.last.y = p1.y;
	
}

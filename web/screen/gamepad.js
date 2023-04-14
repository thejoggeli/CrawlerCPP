function Gamepad(){}
Gamepad.repaintRequired = false;
Gamepad.debugDraw = false;
Gamepad.buttons = {};
Gamepad.sticks = {};
Gamepad.cross = {};
Gamepad.colors = {
	main_button: "#ff0b00",
	main_button_pressed: "yellow",
	main_button_label: "#eee",
	main_button_label_pressed: "#eee",
	outline: "#eee",
	border: "black",
	pad: "#333",
	pad_pressed: "yellow",
	stick: "#eee",
	stick_pressed: "yellow",
	arrows: "black",	
	arrows_pressed: "yellow",
	mid_pad: "#aaa",
};
Gamepad.touchEffects = [];
Gamepad.lineWidth;
Gamepad.createButtons = function(){	
	Gamepad.buttons.up = new GamepadArrowButton({
		label: "up", code: 10, angle:0, keyboardCode: 87,
	});
	Gamepad.buttons.down = new GamepadArrowButton({
		label: "down", code: 11, angle:Math.PI, keyboardCode: 83,
	});	
	Gamepad.buttons.left = new GamepadArrowButton({
		label: "left", code: 12, angle:Math.PI*1.5, keyboardCode: 65,
	});
	Gamepad.buttons.right = new GamepadArrowButton({
		label: "right", code: 13, angle:Math.PI*0.5, keyboardCode: 68,
	}); 
	Gamepad.buttons.a = new GamepadMainButton({
		label: "A", code: 0, keyboardCode: 75,
	});
	Gamepad.buttons.b = new GamepadMainButton({
		label: "B", code: 1, keyboardCode: 76,
	});
	Gamepad.buttons.start = new GamepadMidButton({
		label: "START", code: 20, keyboardCode: 80,
	});
	Gamepad.buttons.select = new GamepadMidButton({
		label: "SELECT", code: 21, keyboardCode: 79,
	}); 
	Gamepad.buttons.menu = new GamepadMidButton({
		label: "HOME", code: -1, keyboardCode: 77,
	}); 
	Gamepad.sticks.left = new GamepadStick({
		code: 0,
	});
	Gamepad.sticks.right = new GamepadStick({
		code: 1,
	});
	Gamepad.sticks.right.enabled = false;
}
Gamepad.init = function(){}
Gamepad.open = function(){
	Haf.install({height:100, container:$("#gamepad")});
	Haf.onResize = Gamepad.resize;
	Haf.onUpdate = Gamepad.update;
	Haf.onRender = Gamepad.render;
	Haf.getCanvas(0).setActive();
	Haf.getCanvas(0).clearColor = "#111";
	Haf.start();
	Haf.focus();
	Gamepad.createButtons();
	Gamepad.recalcButtons();
}
Gamepad.close = function(){
	Haf.uninstall();
}
Gamepad.resize = function(){
	Gamepad.recalcButtons();
	Gamepad.repaintRequired = true;
}
Gamepad.update = function(){
	for(var x in Gamepad.sticks){
		if(!Gamepad.sticks[x].enabled) continue;
		Gamepad.sticks[x].update();	
	}
	for(var x in Gamepad.buttons){
		var px, py;	
		var button = Gamepad.buttons[x];
		var released = false;
		var releasedByHitboxLeave = false;
		var pressed = false;		
		// check if current touch is still active
		if(button.fingerTouch !== null){			
			if(button.fingerTouch.expired){
				// touchend
				button.fingerTouch = null;
				released = true;
			} else {		
				// check if touch left button hitbox
				px = button.fingerTouch.worldPosition.x;
				py = button.fingerTouch.worldPosition.y;
				if(!button.isPointInside(px, py)){
					// yep
					button.fingerTouch.taken = false;
					button.fingerTouch = null;
					released = true;
					releasedByHitboxLeave = true;
				}		
			}		
		}		
		// check if a touch is over button
		if(button.fingerTouch == null){
			for(var t in Input.touches){
				var touch = Input.touches[t];
				if(touch.taken || touch.expired) continue;
				px = touch.worldPosition.x;
				py = touch.worldPosition.y;
				if(button.isPointInside(px, py)){
					// touch over button found!
					pressed = true;
					button.fingerTouch = touch;
					button.fingerTouch.taken = true;
					break;
				}
			}
		}
		if(Input.keyDown(button.keyboardCode)){
			button.keyboardDown = true;
			pressed = true;
		}
		if(Input.keyUp(button.keyboardCode)){
			button.keyboardDown = false;
			released = true;
		}
		if(pressed && released){
			// no change
		} else if(pressed){
			if(button.code == -1){		
				// nothing to do
			} else {
				MatrixClient.sendInput(0, button.code);			
				Gamepad.repaintRequired = true;	
			}
		} else if(released){
			if(button.code == -1 && !releasedByHitboxLeave){
				ScreenManager.open("home");
			} else {
				MatrixClient.sendInput(1, button.code);
				Gamepad.repaintRequired = true;	
			}
		}
		button.update();
	}
	// add new touch effects
	for(var t in Input.newTouches){
		Gamepad.touchEffects.push(new TouchEffect(Input.newTouches[t]));
	}
	// remove expired touch effects
	for(var i = Gamepad.touchEffects.length-1; i >= 0; i--){
		if(Gamepad.touchEffects[i].expired){
			ArrayHelper.removeIndex(Gamepad.touchEffects, i);
		}
	}
	// update touch effects
	for(var x in Gamepad.touchEffects){
		Gamepad.touchEffects[x].update();
	}
	
	Haf.getCanvas(0).autoClear = Gamepad.repaintRequired;	
}
Gamepad.render = function(){	
	if(!Gamepad.repaintRequired){
		return;
	}		
	/*	ctx.font = "16px Arial";
		ctx.fillStyle = "black";
		ctx.textBaseline = "middle";
		ctx.textAlign = "center";
		ctx.fillText("TURN", 0, -27);
		ctx.fillText("YOUR", 0, -9);
		ctx.fillText("PHONE", 0, 9);
		ctx.save();
		ctx.font = "20px Arial";
		ctx.translate(0, 30);
		ctx.rotate(Math.sin(Time.sinceStart)*Math.PI/2+Math.PI/2);
		ctx.fillText(":)", 0, 0);		
		ctx.restore(); */
	if(Gamepad.debugDraw){		
		for(var x in Gamepad.buttons){
			Gamepad.buttons[x].renderDebug();
		}
	}
	for(var x in Gamepad.buttons){
		Gamepad.buttons[x].render();
	}
	var cross = Gamepad.cross;
	var s1 = cross.center_w+cross.border_offset*2;
	var s2 = cross.center_w+cross.border_offset*2+cross.outline_offset*2;
	ctx.fillStyle = Gamepad.colors.border;
	ctx.fillRect(cross.x-s1/2, cross.y-s2/2, s1, s2);
	s1 = cross.center_w;
	s2 = cross.center_w+cross.border_offset*2+cross.outline_offset*2;
	ctx.fillStyle = Gamepad.colors.pad;
	ctx.fillRect(cross.x-s1/2, cross.y-s2/2, s1, s2);
	ctx.fillRect(cross.x-s2/2, cross.y-s1/2, s2, s1);
	for(var x in Gamepad.sticks){
		if(!Gamepad.sticks[x].enabled) continue;
		Gamepad.sticks[x].render();	
	}	
	for(var x in Gamepad.touchEffects){
		Gamepad.touchEffects[x].render();
	}
	ctx.font = "bold " + Haf.height*0.1 + "px Arial";
	ctx.textBaseline = "bottom";
	ctx.textAlign = "left";
	ctx.fillStyle = Gamepad.colors.pad;
	ctx.fillText(Time.fps, -Haf.width/2+1, Haf.height/2);
	if(!MatrixClient.isConnected() && Time.sinceStart%0.8>0.4){
		var cross = Gamepad.cross;
		var left = cross.x - cross.arrow_length - cross.outline_offset - cross.border_offset;
		var top = cross.y - cross.arrow_length - cross.outline_offset - cross.border_offset;
		console.log(cross);
		var radius1 = Gamepad.cross.arrow_length*0.3;
		var radius2 = radius1-cross.outline_offset;
		var radius3 = radius2-cross.border_offset;
		ctx.fillStyle = Gamepad.colors.outline;
		ctx.fillCircle(left + radius1, top+radius3, radius1);		
		ctx.fillStyle = Gamepad.colors.border;
		ctx.fillCircle(left + radius1, top+radius3, radius2);	
		ctx.fillStyle = "red";
		ctx.fillCircle(left + radius1, top+radius3, radius3);		
	}
	Gamepad.repaintRequired = false;
}
Gamepad.recalcButtons = function(){
	var btn = Gamepad.buttons;
	var width = Haf.width;
	var height = Haf.height;	
	
	// cross
	var cross_max_w = width*0.475; 
	var cross_w = height;
	var cross_h = height;
	if(cross_w > cross_max_w){
		cross_w = cross_max_w;
		cross_h = cross_w;
	}
	var center_s = cross_w*0.225;
	var cross_x = -width/2 + cross_w/2;
	var cross_dx = cross_w/2 - center_s/2;
	var cross_dy = cross_h/2 - center_s/2;
	var cross = Gamepad.cross;
	cross.x = cross_x; 
	cross.y = 0;
	cross.w = cross_w;
	cross.h = cross_h;
	cross.center_w = center_s; //cross_w*0.22;
	cross.center_h = center_s; //cross.center_w;
	cross.arrow_length = cross.h*0.44;
	cross.outline_offset = cross.w*0.015;
	cross.border_offset = cross.w*0.0125;
	Gamepad.lineWidth = cross.border_offset;
	// cross center
	btn.up.position.setFloats(cross_x, -center_s/2-cross_dy/2);
	btn.down.position.setFloats(cross_x, +center_s/2+cross_dy/2);
	btn.left.position.setFloats(cross_x-center_s/2-cross_dx/2, 0);
	btn.right.position.setFloats(cross_x+center_s/2+cross_dx/2, 0);
	// cross size
	btn.up.setSize(center_s, cross_w, cross_dy);
	btn.down.setSize(center_s, cross_w, cross_dy);
	btn.left.setSize(center_s, cross_h, cross_dx);
	btn.right.setSize(center_s, cross_h, cross_dx);
	var cross_right = cross_x + cross_w/2;
	var rem_w = width - cross_w;
	
	// stick
	var stick_radius = cross.center_w/2*1.25;
	var stick_max_distance = cross_w*0.425 - stick_radius;
	var stick_center_radius = stick_radius * 0.8;
	var stick = Gamepad.sticks.left;
	stick.setPosition(cross_x, 0);
	stick.radius = stick_radius;
	stick.centerRadius = stick_center_radius;
	stick.maxDistance = stick_max_distance;
	
	stick = Gamepad.sticks.right;
	stick.setPosition(-cross_x, 0);
	stick.radius = stick_radius;
	stick.centerRadius = stick_center_radius;
	stick.maxDistance = stick_max_distance;
	
	// ab
	var ab_w = rem_w/3*2;
	if(ab_w > cross_w) {
		ab_w = cross_w;
	}
	var ab_h = cross_h;
	var ab_x = width/2-ab_w/2;
	btn.a.position.setFloats(ab_x, ab_h/4);
	btn.b.position.setFloats(ab_x, -ab_h/4);
	btn.a.setSize(ab_w, ab_h/2);
	btn.b.setSize(ab_w, ab_h/2);
	
	// mid
	var mid_x = (-width/2 + cross_w + width/2 - ab_w)/2;
	var mid_w = width - cross_w - ab_w;
	var mid_h = cross_h/3;
	btn.start.position.setFloats(mid_x, 0);
	btn.start.setSize(mid_w, mid_h);
	btn.select.position.setFloats(mid_x, cross_h/3);
	btn.select.setSize(mid_w, mid_h);
	btn.menu.position.setFloats(mid_x, -cross_h/3);
	btn.menu.setSize(mid_w, mid_h);
	Gamepad.repaintRequired = true;	
}

function TouchEffect(touch){
	this.touch = touch;
	this.position = new Vector();
	this.oldPositions = [];
	this.oldPositionsTimer = 0;
	this.maxLength = 15;
	this.repaintCounter = 0;
}
TouchEffect.minDistance = 1
TouchEffect.prototype.update = function(){
	if(this.touch.expired){
		this.oldPositionsTimer -= Time.deltaTime;
		if(this.oldPositionsTimer < 0){
			this.oldPositionsTimer = 0.01;
			ArrayHelper.removeIndex(this.oldPositions, 0);
			if(this.oldPositions.length < 2){
				this.expired = true;
			}
		}
		Gamepad.repaintRequired = true;	
	} else {
		this.position.setVector(this.touch.worldPosition);
		this.oldPositionsTimer -= Time.deltaTime;
		while(this.oldPositionsTimer < 0){
			this.oldPositions.push({
				x: this.touch.worldPosition.x,
				y: this.touch.worldPosition.y,
			});
			this.oldPositionsTimer += 1/60;
			if(this.oldPositions.length > this.maxLength){
				ArrayHelper.removeIndex(this.oldPositions, 0);
			}			
		}
		if(Vector.getDistanceBetween(this.oldPositions[0], this.oldPositions[this.oldPositions.length-1]) >= TouchEffect.minDistance){
			this.repaintCounter = 2
		}
	}
	if(this.repaintCounter > 0){
		this.repaintCounter--;
		Gamepad.repaintRequired = true;	
	}
}
TouchEffect.prototype.render = function(){
	if(this.oldPositions.length > 1){
		if(Vector.getDistanceBetween(this.oldPositions[0], this.oldPositions[this.oldPositions.length-1]) < TouchEffect.minDistance){
			return;
		}
		var lineThickness = 2;
		for(var i = 1; i < this.oldPositions.length; i++){
			ctx.beginPath();
			ctx.moveTo(this.oldPositions[i-1].x, this.oldPositions[i-1].y);
			ctx.lineTo(this.oldPositions[i].x, this.oldPositions[i].y);
			ctx.lineWidth = Gamepad.lineWidth * (i/(this.maxLength-1)) * lineThickness;
			ctx.strokeStyle = "white";
			ctx.stroke();
			ctx.fillStyle = "white";			
			ctx.fillCircle(this.oldPositions[i].x, this.oldPositions[i].y, ctx.lineWidth/2);
		}
	}
}

function GamepadStick(params){
	this.enabled = true;
	this.center = new Vector();
	this.centerRadius;
	this.position = new Vector();
	this.touchOffset = new Vector();
	this.releasePosition = new Vector();
	this.releaseTimer = 1;
	this.releaseTimerScale = 1;
	this.radius = 5;
	this.touch = null;
	this.maxDistance;
	this.code = params.code;
	this.sendMoveTimer = 0;
	this.sendMoveInterval = 0.05;
}
GamepadStick.prototype.update = function(){
	var old_x = this.position.x;
	var old_y = this.position.y;
	for(var i in Input.newTouches){
		var touch = Input.newTouches[i];
		if(touch.taken) continue;
		var px = touch.worldPosition.x;
		var py = touch.worldPosition.y;
		if(CollisionManager.isPointInsideCircle(px, py, this.position.x, this.position.y, this.radius)){
			this.touch = touch;
		 	touch.taken = true;
		 	this.touchOffset.x = this.position.x - this.touch.worldPosition.x;
		 	this.touchOffset.y = this.position.y - this.touch.worldPosition.y;
		 	this.sendMoveTimer = 0;
		 	// send joystick press event 
			MatrixClient.sendJoystick(0, this.code); 		
			Gamepad.repaintRequired = true;	 	
		}
	}
	if(this.touch != null){
		if(this.touch.expired){
			this.touch = null;
			this.releasePosition.setVector(this.position);
			this.releaseTimer = 0;
			this.releaseTimerScale = 1/Vector.getDistanceBetween(this.position, this.center)*200;
			this.touchOffset.setFloats(0, 0);
			// send joystick release event
			MatrixClient.sendJoystick(1, this.code); 
			Gamepad.repaintRequired = true;	
		} else {
			var ox = this.position.x;
			var oy = this.position.y;
			this.position.setVector(this.touch.worldPosition);
			this.position.addVector(this.touchOffset);
			// slowly move offset towards zero
			var dx = Math.abs(this.position.x - ox);
			var dy = Math.abs(this.position.y - oy);
			if(dx > 0) this.touchOffset.x *= 1-dx/(this.maxDistance*2);
			if(dy > 0) this.touchOffset.y *= 1-dy/(this.maxDistance*2);
		}
	}
	if(this.touch == null){
		this.releaseTimer += Time.deltaTime * this.releaseTimerScale;
		this.position.x = Numbers.lerp(this.releasePosition.x, this.center.x, this.releaseTimer);
		this.position.y = Numbers.lerp(this.releasePosition.y, this.center.y, this.releaseTimer);
	}
	this.distance = Vector.getDistanceBetween(this.position, this.center);
	if(this.distance > this.maxDistance){	
		var angle = Vector.getAngleBetween(this.center, this.position);
		this.position.x = this.center.x + Math.cos(angle)*this.maxDistance;
		this.position.y = this.center.y + Math.sin(angle)*this.maxDistance;
	}
	if(this.isTouched()){
		if(this.sendMoveTimer <= 0){
			this.sendMoveTimer += this.sendMoveInterval;
			var dead = 0.2;
			var relative_x = (this.position.x - this.center.x) / this.maxDistance;
			var relative_y = (this.position.y - this.center.y) / this.maxDistance;
			relative_x = this.deadzone(relative_x, 0.05);
			relative_y = this.deadzone(relative_y, 0.05);
			// send joystick move event
			MatrixClient.sendJoystick(2, this.code, roundToFixed(relative_x, 6), roundToFixed(-relative_y, 6));
			Gamepad.repaintRequired = true;	
		}
		this.sendMoveTimer -= Time.deltaTime;
	}
	if(old_x != this.position.x || old_y != this.position.y){		
		Gamepad.repaintRequired = true;
	}
}
GamepadStick.prototype.deadzone = function(val, dead){
	if(val > dead){
		return val*(1+dead)-dead;
	} else if(val < -dead){
		return val*(1+dead)+dead;
	}	
	return 0;					 
}
GamepadStick.prototype.render = function(){
	// draw line
	ctx.lineWidth = this.radius*0.5;
	ctx.strokeStyle = "black";
	ctx.strokeLine(this.center.x, this.center.y, this.position.x, this.position.y);
	ctx.fillStyle = "black";
	ctx.fillCircle(this.center.x, this.center.y, ctx.lineWidth/2);
	ctx.lineWidth = Gamepad.lineWidth;
	ctx.strokeCircle(this.center.x, this.center.y, this.radius);
	// draw cirlce
	var cross = Gamepad.cross;
	ctx.lineWidth = Gamepad.lineWidth;
	ctx.fillStyle = this.isTouched() ? Gamepad.colors.stick_pressed : Gamepad.colors.stick;
	ctx.fillCircle(this.position.x, this.position.y, this.radius);
/*	if(!this.isTouched() && this.releaseTimer < 1){
		ctx.globalAlpha = 1-(this.releaseTimer < 1 ? this.releaseTimer : 0);
		ctx.fillStyle = Gamepad.colors.pad_pressed;
		ctx.fillCircle(this.position.x, this.position.y, this.radius);
		ctx.globalAlpha = 1;
	} */
	ctx.strokeStyle = Gamepad.colors.arrows;
	ctx.strokeCircle(this.position.x, this.position.y, this.radius);

	
}
GamepadStick.prototype.setPosition = function(x, y){
	this.center.setFloats(x, y);
	this.position.setFloats(x, y);
	this.releasePosition.setFloats(x, y);
}
GamepadStick.prototype.isTouched = function(){
	return this.touch == null ? false : true;
}

function GamepadArrowButton(params){
	this.position = new Vector();
	this.win; this.wout; this.h;
	this.angle = params.angle;
	this.label = params.label; 
	this.code = params.code;
	this.points;
	this.fingerTouch = null;
	this.keyboardDown = false;
	this.keyboardCode = params.keyboardCode;
}
GamepadArrowButton.prototype.update = function(){}
GamepadArrowButton.prototype.renderDebug = function(){
	// debug draw
	ctx.beginPath();
	ctx.lineWidth = Gamepad.lineWidth;
	ctx.strokeStyle = "black";
	ctx.fillStyle = this.isPressed() ? "yellow" : "blue";
	for(var i in this.points){
		ctx.lineTo(this.points[i][0], this.points[i][1]);
	}
	ctx.closePath();
	ctx.fill();
	ctx.stroke(); 
}
GamepadArrowButton.prototype.render = function(){
	// cross draw
	var cross = Gamepad.cross;
	var border_offset = cross.border_offset;
	var outline_offset = cross.outline_offset;
	var width = cross.center_w;
	var height = cross.arrow_length;
	var radius = cross.center_w*0.1;
	ctx.save();
	ctx.translate(cross.x, cross.y);
	ctx.rotate(this.angle+Math.PI);
	// outline
	var w1 = cross.center_w + outline_offset*2 + border_offset*2;
	var h1 = height + outline_offset + border_offset;
	ctx.fillStyle = Gamepad.colors.outline;
	ctx.fillRoundRect(0-w1/2, 0, w1, h1, radius);	
	// border
	var w2 = cross.center_w + border_offset*2;
	var h2 = height + border_offset;
	ctx.fillStyle = Gamepad.colors.border;
	ctx.fillRoundRect(0-w2/2, 0, w2, h2, radius);	
	// pad
	var w3 = cross.center_w;
	var h3 = height;
	ctx.fillStyle = Gamepad.colors.pad;
	ctx.fillRoundRect(0-w3/2, 0, w3, h3, radius);
	// pad touched
	if(this.isPressed()){
		var h4 = h3*0.62;
		ctx.fillStyle = Gamepad.colors.pad_pressed;
		ctx.fillRoundRect(0-w3/2, h3-h4, w3, h4, radius);
		ctx.fillRect(0-w3/2, h3-h4, w3, radius, radius);
	}
	// arrow
	ctx.lineWidth = Gamepad.lineWidth;
	var ay1 = height*0.93;
	var ay2 = height*0.7;
	var ay3 = height*0.5;
	var aw2 = width*0.7;
	var aw3 = width*0.5;
	ctx.beginPath();
	ctx.lineTo(0, ay1);
	ctx.lineTo(aw2/2, ay2);
	ctx.lineTo(aw3/2, ay2);
	ctx.lineTo(aw3/2, ay3);
	ctx.lineTo(-aw3/2, ay3);
	ctx.lineTo(-aw3/2, ay2);
	ctx.lineTo(-aw2/2, ay2);
	ctx.closePath();
	if(this.isPressed()){
		ctx.fillStyle = Gamepad.colors.arrows_pressed;
		ctx.fill();
	}
	ctx.strokeStyle = Gamepad.colors.arrows;
	ctx.stroke();
	ctx.restore();
}
GamepadArrowButton.prototype.setSize = function(win, wout, h){
	this.win = win;
	this.wout = wout;
	this.h = h;
	var cosa = Math.cos(this.angle);
	var sina = Math.sin(this.angle);
	this.points = [];
	this.points[0] = [-this.wout/2, -this.h/2];
	this.points[1] = [this.wout/2, -this.h/2];
	this.points[2] = [this.win/2, this.h/2];
	this.points[3] = [-this.win/2, this.h/2];
	for(var i in this.points){
		var x = this.points[i][0];
		var y = this.points[i][1];
		this.points[i][0] = x*cosa - y*sina;
		this.points[i][1] = x*sina + y*cosa;
		this.points[i][0] += this.position.x;
		this.points[i][1] += this.position.y;
	}
}
GamepadArrowButton.prototype.isTouched = function(){
	return this.fingerTouch != null;
}
GamepadArrowButton.prototype.isPressed = function(){
	return this.fingerTouch != null || this.keyboardDown;
}
GamepadArrowButton.prototype.isPointInside = function(x, y){
	return CollisionManager.isPointInsidePolygon(x, y, this.points);
}

function GamepadMainButton(params){
	this.position = new Vector();
	this.width = 25;
	this.height = 25;
	this.label = params.label; 
	this.code = params.code;
	this.fingerTouch = null;
	this.keyboardDown = false;
	this.keyboardCode = params.keyboardCode;
}
GamepadMainButton.prototype.update = function(){

}
GamepadMainButton.prototype.renderDebug = function(){
	ctx.fillStyle = this.isPressed() ? "yellow" : "green";
	ctx.lineWidth = Gamepad.lineWidth;
	ctx.fillRect(this.position.x-this.width/2, this.position.y-this.height/2, this.width, this.height);	
	ctx.strokeStyle = "black";
	ctx.strokeRect(this.position.x-this.width/2, this.position.y-this.height/2, this.width, this.height);
}
GamepadMainButton.prototype.render = function(){	
	var w = this.width*0.92;
	var h = this.height*0.9;
	var min = w < h ? w : h;
	var r1 = w*0.1;
	var x = this.position.x; // Haf.width/2 - this.height/2;
	ctx.save();
	ctx.translate(x,this.position.y);
	// background
	ctx.fillStyle = Gamepad.colors.outline;
	ctx.fillRoundRect(-w/2, -h/2, w, h, r1);
	var button_radius = min/2*0.8;
	ctx.fillStyle = this.isPressed() ? Gamepad.colors.main_button_pressed : Gamepad.colors.main_button;
	ctx.fillCircle(0, 0, button_radius);
	// label
	ctx.font = "bold " + min*0.6+ "px Arial";
	ctx.textAlign = "center";
	ctx.textBaseline = "middle";
	ctx.fillStyle = this.isPressed() ? Gamepad.colors.main_button_label_pressed : Gamepad.colors.main_button_label;
	ctx.fillText(this.label, 0, 0);
	ctx.restore();
}
GamepadMainButton.prototype.setSize = function(w, h){
	this.width = w;
	this.height = h;
}
GamepadMainButton.prototype.isTouched = function(){
	return this.fingerTouch != null;
}
GamepadMainButton.prototype.isPressed = function(){
	return this.fingerTouch != null || this.keyboardDown;
}
GamepadMainButton.prototype.isPointInside = function(x, y){
	return CollisionManager.isPointInsideRectangle(x, y, this.position.x, this.position.y, this.width, this.height);
}

function GamepadMidButton(params){
	this.position = new Vector();
	this.width = 25;
	this.height = 25;
	this.label = params.label; 
	this.code = params.code;
	this.fingerTouch = null;
	this.keyboardDown = false;
	this.keyboardCode = params.keyboardCode;
}
GamepadMidButton.prototype.update = function(){
	
}
GamepadMidButton.prototype.renderDebug = function(){
	ctx.fillStyle = this.isPressed() ? "yellow" : "cyan";
	ctx.lineWidth = Gamepad.lineWidth;
	ctx.fillRect(this.position.x-this.width/2, this.position.y-this.height/2, this.width, this.height);	
	ctx.strokeStyle = "black";
	ctx.strokeRect(this.position.x-this.width/2, this.position.y-this.height/2, this.width, this.height);
}
GamepadMidButton.prototype.render = function(){
	var cross = Gamepad.cross;	
	var border_offset = cross.border_offset;
	var outline_offset = cross.outline_offset;
	var h = cross.center_w + border_offset*2 + outline_offset*2;
	var w = this.width*0.9;
	if(h > this.height*0.9){
		h = this.height*0.9;
	}
	var min = h < w ? h : w;
	var r1 = min*0.1;
	ctx.save();
	ctx.translate(this.position.x, this.position.y);
	
	// outline
	ctx.fillStyle = Gamepad.colors.outline;
	ctx.fillRoundRect(-w/2, -h/2, w, h, r1);
	
	// border
	var w1 = w - outline_offset*2;
	var h1 = h - outline_offset*2;
	ctx.fillStyle = Gamepad.colors.border;
	ctx.fillRoundRect(-w1/2, -h1/2, w1, h1, r1);	
	
	// pad
	var w2 = w1 - border_offset*2;
	var h2 = h1 - border_offset*2;
	ctx.fillStyle = this.isPressed() ? Gamepad.colors.pad_pressed : Gamepad.colors.mid_pad;
	ctx.fillRoundRect(-w2/2, -h2/2, w2, h2, r1);
	
	// label
	ctx.font = "bold " + min*0.2 + "px Arial";
	ctx.textBaseline = "middle";
	ctx.textAlign = "center";
	ctx.fillStyle = "black";
	ctx.fillText(this.label, 0, 0);	
		
	ctx.restore();
}
GamepadMidButton.prototype.isTouched = function(){
	return this.fingerTouch != null;
}
GamepadMidButton.prototype.isPressed = function(){
	return this.fingerTouch != null || this.keyboardDown;
}
GamepadMidButton.prototype.isPointInside = function(x, y){
	return CollisionManager.isPointInsideRectangle(x, y, this.position.x, this.position.y, this.width, this.height);
}
GamepadMidButton.prototype.setSize = function(w, h){
	this.width = w;
	this.height = h;
}



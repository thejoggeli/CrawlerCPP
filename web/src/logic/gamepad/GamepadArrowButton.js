import CollisionDetection from "msl/util/CollisionDetection";
import Vec2 from "msl/util/Vec2";

function GamepadArrowButton(gamepad, params){
    this.gamepad = gamepad
	this.position = new Vec2();
	this.win; this.wout; this.h;
	this.angle = params.angle;
	this.label = params.label; 
	this.code = params.code;
	this.points;
	this.fingerTouch = null;
	this.keyboardDown = false;
	this.keyboardCode = params.keyboardCode;
}
GamepadArrowButton.prototype.update = function(canvas){}
GamepadArrowButton.prototype.renderDebug = function(canvas){
    var ctx = canvas.ctx;
	// debug draw
	ctx.beginPath();
	ctx.lineWidth = this.gamepad.lineWidth;
	ctx.strokeStyle = "black";
	ctx.fillStyle = this.isPressed() ? "yellow" : "blue";
	for(var i in this.points){
		ctx.lineTo(this.points[i][0], this.points[i][1]);
	}
	ctx.closePath();
	ctx.fill();
	ctx.stroke(); 
}
GamepadArrowButton.prototype.render = function(canvas){
    var ctx = canvas.ctx;
	// cross draw
	var cross = this.gamepad.cross;
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
	ctx.fillStyle = this.gamepad.colors.outline;
	ctx.fillRoundRect(0-w1/2, 0, w1, h1, radius);	
	// border
	var w2 = cross.center_w + border_offset*2;
	var h2 = height + border_offset;
	ctx.fillStyle = this.gamepad.colors.border;
	ctx.fillRoundRect(0-w2/2, 0, w2, h2, radius);	
	// pad
	var w3 = cross.center_w;
	var h3 = height;
	ctx.fillStyle = this.gamepad.colors.pad;
	ctx.fillRoundRect(0-w3/2, 0, w3, h3, radius);
	// pad touched
	if(this.isPressed()){
		var h4 = h3*0.62;
		ctx.fillStyle = this.gamepad.colors.pad_pressed;
		ctx.fillRoundRect(0-w3/2, h3-h4, w3, h4, radius);
		ctx.fillRect(0-w3/2, h3-h4, w3, radius, radius);
	}
	// arrow
	ctx.lineWidth = this.gamepad.lineWidth;
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
		ctx.fillStyle = this.gamepad.colors.arrows_pressed;
		ctx.fill();
	}
	ctx.strokeStyle = this.gamepad.colors.arrows;
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
	return CollisionDetection.isPointInsidePolygon(x, y, this.points);
}

export default GamepadArrowButton
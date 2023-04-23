import CollisionDetection from "msl/util/CollisionDetection";
import Vec2 from "msl/util/Vec2";

function GamepadMidButton(gamepad, params){
    this.gamepad = gamepad
	this.position = new Vec2();
	this.width = 25;
	this.height = 25;
	this.label = params.label; 
	this.code = params.code;
	this.fingerTouch = null;
	this.keyboardDown = false;
	this.keyboardCode = params.keyboardCode;
}
GamepadMidButton.prototype.update = function(canvas){
	
}
GamepadMidButton.prototype.renderDebug = function(canvas){
    var ctx = canvas.ctx;
	ctx.fillStyle = this.isPressed() ? "yellow" : "cyan";
	ctx.lineWidth = this.gamepad.lineWidth;
	ctx.fillRect(this.position.x-this.width/2, this.position.y-this.height/2, this.width, this.height);	
	ctx.strokeStyle = "black";
	ctx.strokeRect(this.position.x-this.width/2, this.position.y-this.height/2, this.width, this.height);
}
GamepadMidButton.prototype.render = function(canvas){
    var ctx = canvas.ctx;
	var cross = this.gamepad.cross;	
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
	ctx.fillStyle = this.gamepad.colors.outline;
	ctx.fillRoundRect(-w/2, -h/2, w, h, r1);
	
	// border
	var w1 = w - outline_offset*2;
	var h1 = h - outline_offset*2;
	ctx.fillStyle = this.gamepad.colors.border;
	ctx.fillRoundRect(-w1/2, -h1/2, w1, h1, r1);	
	
	// pad
	var w2 = w1 - border_offset*2;
	var h2 = h1 - border_offset*2;
	ctx.fillStyle = this.isPressed() ? this.gamepad.colors.pad_pressed : this.gamepad.colors.mid_pad;
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
	return CollisionDetection.isPointInsideRectangle(x, y, this.position.x, this.position.y, this.width, this.height);
}
GamepadMidButton.prototype.setSize = function(w, h){
	this.width = w;
	this.height = h;
}



export default GamepadMidButton
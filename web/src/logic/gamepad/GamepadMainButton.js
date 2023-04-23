import CollisionDetection from "msl/util/CollisionDetection";
import Vec2 from "msl/util/Vec2";

function GamepadMainButton(gamepad, params){
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
GamepadMainButton.prototype.update = function(canvas){

}
GamepadMainButton.prototype.renderDebug = function(canvas){
    var ctx = canvas.ctx;
	ctx.fillStyle = this.isPressed() ? "yellow" : "green";
	ctx.lineWidth = this.gamepad.lineWidth;
	ctx.fillRect(this.position.x-this.width/2, this.position.y-this.height/2, this.width, this.height);	
	ctx.strokeStyle = "black";
	ctx.strokeRect(this.position.x-this.width/2, this.position.y-this.height/2, this.width, this.height);
}
GamepadMainButton.prototype.render = function(canvas){
    var ctx = canvas.ctx;	
	var w = this.width*0.92;
	var h = this.height*0.9;
	var min = w < h ? w : h;
	var r1 = w*0.1;
	var x = this.position.x; 
	ctx.save();
	ctx.translate(x,this.position.y);
	// background
	ctx.fillStyle = this.gamepad.colors.outline;
	ctx.fillRoundRect(-w/2, -h/2, w, h, r1);
	var button_radius = min/2*0.8;
	ctx.fillStyle = this.isPressed() ? this.gamepad.colors.main_button_pressed : this.gamepad.colors.main_button;
	ctx.fillCircle(0, 0, button_radius);
	// label
	ctx.font = "bold " + min*0.6+ "px Arial";
	ctx.textAlign = "center";
	ctx.textBaseline = "middle";
	ctx.fillStyle = this.isPressed() ? this.gamepad.colors.main_button_label_pressed : this.gamepad.colors.main_button_label;
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
	return CollisionDetection.isPointInsideRectangle(x, y, this.position.x, this.position.y, this.width, this.height);
}

export default GamepadMainButton
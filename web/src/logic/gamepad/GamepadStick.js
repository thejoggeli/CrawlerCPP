import Time from "msl/time/Time";
import CollisionDetection from "msl/util/CollisionDetection";
import Numbers from "msl/util/Numbers";
import Vec2 from "msl/util/Vec2";

function GamepadStick(gamepad, params){
    this.gamepad = gamepad
	this.enabled = true;
	this.center = new Vec2();
	this.centerRadius;
	this.position = new Vec2();
	this.touchOffset = new Vec2();
	this.releasePosition = new Vec2();
	this.releaseTimer = 1;
	this.releaseTimerScale = 1;
	this.radius = 5;
	this.touch = null;
	this.maxDistance;
	this.code = params.code;
	this.sendMoveTimer = 0;
	this.sendMoveFrequency = 40;
	this.sendMoveInterval = 1.0 / this.sendMoveFrequency;
}
GamepadStick.prototype.update = function(canvas){
	var old_x = this.position.x;
	var old_y = this.position.y;
	for(var i in canvas.input.newTouches){
		var touch = canvas.input.newTouches[i];
		if(touch.taken) continue;
		var px = touch.worldPosition.x;
		var py = touch.worldPosition.y;
		if(CollisionDetection.isPointInsideCircle(px, py, this.position.x, this.position.y, this.radius)){
			this.touch = touch;
		 	touch.taken = true;
		 	this.touchOffset.x = this.position.x - this.touch.worldPosition.x;
		 	this.touchOffset.y = this.position.y - this.touch.worldPosition.y;
		 	this.sendMoveTimer = 0;
		 	// send joystick press event 
            this.gamepad.notifySubscribers("onButtonPress", {id: this.code})
			this.gamepad.repaintRequired = true;	 	
		}
	}
	if(this.touch != null){
		if(this.touch.expired){
			this.touch = null;
            Vec2.copy(this.releasePosition, this.position)
			this.releaseTimer = 0;
			this.releaseTimerScale = 1/Vec2.getDistance(this.position, this.center)*200;
            Vec2.set(this.touchOffset, 0.0, 0.0)
			// send joystick release event
            this.gamepad.notifySubscribers("onButtonRelease", {id: this.code}) 
			this.gamepad.repaintRequired = true;	
		} else {
			var ox = this.position.x;
			var oy = this.position.y;
            Vec2.copy(this.position, this.touch.worldPosition);
            Vec2.add(this.position, this.position, this.touchOffset)
			// slowly move offset towards zero
			var dx = Math.abs(this.position.x - ox);
			var dy = Math.abs(this.position.y - oy);
			if(dx > 0) this.touchOffset.x *= 1-dx/(this.maxDistance*2);
			if(dy > 0) this.touchOffset.y *= 1-dy/(this.maxDistance*2);
		}
	}
	if(this.touch == null){
		this.releaseTimer += Time.deltaTime * this.releaseTimerScale;
		this.position.x = Numbers.lerpClamp(this.releasePosition.x, this.center.x, this.releaseTimer);
		this.position.y = Numbers.lerpClamp(this.releasePosition.y, this.center.y, this.releaseTimer);
	}
    this.distance = Vec2.getDistance(this.position, this.center)
	if(this.distance > this.maxDistance){	
        var angle = Vec2.getAngleBetween(this.center, this.position)
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
            var event = {id: this.code, x: relative_x, y: relative_y}
            this.gamepad.notifySubscribers("onJoystickMove", event)
			this.gamepad.repaintRequired = true;	
		}
		this.sendMoveTimer -= Time.deltaTime;
	}
	if(old_x != this.position.x || old_y != this.position.y){		
		this.gamepad.repaintRequired = true;
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
GamepadStick.prototype.render = function(canvas){
    var ctx = canvas.ctx;
	// draw line
	ctx.lineWidth = this.radius*0.5;
	ctx.strokeStyle = "black";
	ctx.strokeLine(this.center.x, this.center.y, this.position.x, this.position.y);
	ctx.fillStyle = "black";
	ctx.fillCircle(this.center.x, this.center.y, ctx.lineWidth/2);
	ctx.lineWidth = this.gamepad.lineWidth;
	ctx.strokeCircle(this.center.x, this.center.y, this.radius);
	// draw cirlce
	var cross = this.gamepad.cross;
	ctx.lineWidth = this.gamepad.lineWidth;
	ctx.fillStyle = this.isTouched() ? this.gamepad.colors.stick_pressed : this.gamepad.colors.stick;
	ctx.fillCircle(this.position.x, this.position.y, this.radius);
/*	if(!this.isTouched() && this.releaseTimer < 1){
		ctx.globalAlpha = 1-(this.releaseTimer < 1 ? this.releaseTimer : 0);
		ctx.fillStyle = this.gamepad.colors.pad_pressed;
		ctx.fillCircle(this.position.x, this.position.y, this.radius);
		ctx.globalAlpha = 1;
	} */
	ctx.strokeStyle = this.gamepad.colors.arrows;
	ctx.strokeCircle(this.position.x, this.position.y, this.radius);

	
}
GamepadStick.prototype.setPosition = function(x, y){
    Vec2.set(this.center, x, y);
    Vec2.set(this.position, x, y);
    Vec2.set(this.releasePosition, x, y);
}
GamepadStick.prototype.isTouched = function(){
	return this.touch == null ? false : true;
}


export default GamepadStick
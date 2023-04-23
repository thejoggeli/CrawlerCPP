import Time from "msl/time/Time";
import Arrays from "msl/util/Arrays";
import Vec2 from "msl/util/Vec2";

function GamepadTouchEffect(gamepad, touch){
    this.gamepad = gamepad
	this.touch = touch;
	this.position = new Vec2(0.0, 0.0);
	this.oldPositions = [];
	this.oldPositionsTimer = 0;
	this.maxLength = 15;
	this.repaintCounter = 0;
}
GamepadTouchEffect.minDistance = 1
GamepadTouchEffect.prototype.update = function(canvas){
	if(this.touch.expired){
		this.oldPositionsTimer -= Time.deltaTime;
		if(this.oldPositionsTimer < 0){
			this.oldPositionsTimer = 0.01;
			Arrays.removeIndex(this.oldPositions, 0);
			if(this.oldPositions.length < 2){
				this.expired = true;
			}
		}
		this.gamepad.repaintRequired = true;	
	} else {
		Vec2.copy(this.position, this.touch.worldPosition)
		this.oldPositionsTimer -= Time.deltaTime;
		while(this.oldPositionsTimer < 0){
			this.oldPositions.push({
				x: this.touch.worldPosition.x,
				y: this.touch.worldPosition.y,
			});
			this.oldPositionsTimer += 1/60;
			if(this.oldPositions.length > this.maxLength){
				Arrays.removeIndex(this.oldPositions, 0);
			}			
		}
		if(Vec2.getDistance(this.oldPositions[0], this.oldPositions[this.oldPositions.length-1]) >= GamepadTouchEffect.minDistance){
			this.repaintCounter = 2
		}
	}
	if(this.repaintCounter > 0){
		this.repaintCounter--;
		this.gamepad.repaintRequired = true;	
	}
}
GamepadTouchEffect.prototype.render = function(canvas){
    var ctx = canvas.ctx;
	if(this.oldPositions.length > 1){
		if(Vec2.getDistance(this.oldPositions[0], this.oldPositions[this.oldPositions.length-1]) < GamepadTouchEffect.minDistance){
			return;
		}
		var lineThickness = 2;
		for(var i = 1; i < this.oldPositions.length; i++){
			ctx.beginPath();
			ctx.moveTo(this.oldPositions[i-1].x, this.oldPositions[i-1].y);
			ctx.lineTo(this.oldPositions[i].x, this.oldPositions[i].y);
			ctx.lineWidth = this.gamepad.lineWidth * (i/(this.maxLength-1)) * lineThickness;
			ctx.strokeStyle = "white";
			ctx.stroke();
			ctx.fillStyle = "white";			
			ctx.fillCircle(this.oldPositions[i].x, this.oldPositions[i].y, ctx.lineWidth/2);
		}
	}
}

export default GamepadTouchEffect
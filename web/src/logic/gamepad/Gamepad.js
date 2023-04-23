import GamepadMainButton from "./GamepadMainButton";
import GamepadArrowButton from "./GamepadArrowButton";
import GamepadStick from "./GamepadStick";
import GamepadMidButton from "./GamepadMidButton";
import GamepadTouchEffect from "./GamepadTouchEffect";
import Gfw from "msl/gfw/Gfw";
import Vec2 from "msl/util/Vec2";
import Time from "msl/time/Time";
import Arrays from "msl/util/Arrays";
import Numbers from "msl/util/Numbers";
import Subscribable from "msl/util/Subscribable";

export default class Gamepad extends Subscribable {

	constructor(){
		super();
		this.canvas = null;
		this.repaintRequired = false;
		this.debugDraw = false;
		this.buttons = {};
		this.sticks = {};
		this.cross = {};
		this.colors = {
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
		this.touchEffects = [];
		this.lineWidth;
	}
	install(container){
		this.canvas = Gfw.createCanvas("gamepad", {
			container: container,
			size: 100,
			clearColor: "#111",
		})
		this.canvas.subscribe("resize", this, this.resize)
		this.canvas.subscribe("update", this, this.update)
		this.canvas.subscribe("render", this, this.render)
		this.canvas.focus();
		this.createButtons();
		this.recalcButtons();
	}
	uninstall(){
		this.canvas.unsubscribe("resize", this, this.resize)
		this.canvas.unsubscribe("update", this, this.update)
		this.canvas.unsubscribe("render", this, this.render)
		Gfw.destroyCanvas(this.canvas.id)
	}
	createButtons(){	
		this.buttons.up = new GamepadArrowButton(this, {
			label: "up", code: 0x20, angle:0, keyboardCode: 87,
		});
		this.buttons.down = new GamepadArrowButton(this, {
			label: "down", code: 0x21, angle:Math.PI, keyboardCode: 83,
		});	
		this.buttons.left = new GamepadArrowButton(this, {
			label: "left", code: 0x22, angle:Math.PI*1.5, keyboardCode: 65,
		});
		this.buttons.right = new GamepadArrowButton(this, {
			label: "right", code: 0x23, angle:Math.PI*0.5, keyboardCode: 68,
		}); 
		this.buttons.a = new GamepadMainButton(this, {
			label: "A", code: 0x10, keyboardCode: 75,
		});
		this.buttons.b = new GamepadMainButton(this, {
			label: "B", code: 0x11, keyboardCode: 76,
		});
		this.buttons.start = new GamepadMidButton(this, {
			label: "START", code: 0x40, keyboardCode: 80,
		});
		this.buttons.select = new GamepadMidButton(this, {
			label: "SELECT", code: 0x41, keyboardCode: 79,
		}); 
		this.buttons.menu = new GamepadMidButton(this, {
			label: "HOME", code: 0, keyboardCode: 77,
		}); 
		this.sticks.left = new GamepadStick(this, {
			code: 0xF0,
		});
		this.sticks.right = new GamepadStick(this, {
			code: 0xF1,
		});
		this.sticks.right.enabled = false;
	}
	resize(canvas){
		this.recalcButtons();
		this.repaintRequired = true;
	}
	update(canvas){
		for(var x in this.sticks){
			if(!this.sticks[x].enabled) continue;
			this.sticks[x].update(canvas);	
		}
		for(var x in this.buttons){
			var px, py;	
			var button = this.buttons[x];
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
				for(var t in canvas.input.touches){
					var touch = canvas.input.touches[t];
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
			if(canvas.input.keyDown(button.keyboardCode)){
				button.keyboardDown = true;
				pressed = true;
			}
			if(canvas.input.keyUp(button.keyboardCode)){
				button.keyboardDown = false;
				released = true;
			}
			if(pressed && released){
				// no change
			} else if(pressed){
				if(button.code == -1){		
					// nothing to do
				} else {
					this.notifySubscribers("onButtonPress", {id: button.code})	
					this.repaintRequired = true;	
				}
			} else if(released){
				this.notifySubscribers("onButtonRelease", {id: button.code})
				this.repaintRequired = true;	
			}
			button.update(canvas);
		}
		// add new touch effects
		for(var t in canvas.input.newTouches){
			this.touchEffects.push(new GamepadTouchEffect(this, canvas.input.newTouches[t]));
		}
		// remove expired touch effects
		for(var i = this.touchEffects.length-1; i >= 0; i--){
			if(this.touchEffects[i].expired){
				Arrays.removeIndex(this.touchEffects, i);
			}
		}
		// update touch effects
		for(var x in this.touchEffects){
			this.touchEffects[x].update(canvas);
		}
		
		this.canvas.autoClear = this.repaintRequired;	
	}
	render(canvas){	
		if(!this.repaintRequired){
			return;
		}		
		if(this.debugDraw){		
			for(var x in this.buttons){
				this.buttons[x].renderDebug(canvas);
			}
		}
		for(var x in this.buttons){
			this.buttons[x].render(canvas);
		}
		var ctx = canvas.ctx
		var cross = this.cross;
		var s1 = cross.center_w+cross.border_offset*2;
		var s2 = cross.center_w+cross.border_offset*2+cross.outline_offset*2;
		ctx.fillStyle = this.colors.border;
		ctx.fillRect(cross.x-s1/2, cross.y-s2/2, s1, s2);
		s1 = cross.center_w;
		s2 = cross.center_w+cross.border_offset*2+cross.outline_offset*2;
		ctx.fillStyle = this.colors.pad;
		ctx.fillRect(cross.x-s1/2, cross.y-s2/2, s1, s2);
		ctx.fillRect(cross.x-s2/2, cross.y-s1/2, s2, s1);
		for(var x in this.sticks){
			if(!this.sticks[x].enabled) continue;
			this.sticks[x].render(canvas);	
		}	
		for(var x in this.touchEffects){
			this.touchEffects[x].render(canvas);
		}
		ctx.font = "bold " + this.canvas.height*0.1 + "px Arial";
		ctx.textBaseline = "bottom";
		ctx.textAlign = "left";
		ctx.fillStyle = this.colors.pad;
		var fps = Numbers.roundToFixed(Gfw.fps, 0)
		ctx.fillText(fps, -this.canvas.width/2+1, this.canvas.height/2);
		this.repaintRequired = false;
	}
	recalcButtons(){
		var btn = this.buttons;
		var width = this.canvas.width;
		var height = this.canvas.height;	
		
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
		var cross = this.cross;
		cross.x = cross_x; 
		cross.y = 0;
		cross.w = cross_w;
		cross.h = cross_h;
		cross.center_w = center_s; //cross_w*0.22;
		cross.center_h = center_s; //cross.center_w;
		cross.arrow_length = cross.h*0.44;
		cross.outline_offset = cross.w*0.015;
		cross.border_offset = cross.w*0.0125;
		this.lineWidth = cross.border_offset;
		// cross center
		Vec2.set(btn.up.position, cross_x, -center_s/2-cross_dy/2);
		Vec2.set(btn.down.position, cross_x, +center_s/2+cross_dy/2);
		Vec2.set(btn.left.position, cross_x-center_s/2-cross_dx/2, 0);
		Vec2.set(btn.right.position, cross_x+center_s/2+cross_dx/2, 0);
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
		var stick = this.sticks.left;
		stick.setPosition(cross_x, 0);
		stick.radius = stick_radius;
		stick.centerRadius = stick_center_radius;
		stick.maxDistance = stick_max_distance;
		
		stick = this.sticks.right;
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
		Vec2.set(btn.a.position, ab_x, ab_h/4);
		Vec2.set(btn.b.position, ab_x, -ab_h/4);
		btn.a.setSize(ab_w, ab_h/2);
		btn.b.setSize(ab_w, ab_h/2);
		
		// mid
		var mid_x = (-width/2 + cross_w + width/2 - ab_w)/2;
		var mid_w = width - cross_w - ab_w;
		var mid_h = cross_h/3;
		Vec2.set(btn.start.position, mid_x, 0);
		btn.start.setSize(mid_w, mid_h);
		Vec2.set(btn.select.position, mid_x, cross_h/3);
		btn.select.setSize(mid_w, mid_h);
		Vec2.set(btn.menu.position, mid_x, -cross_h/3);
		btn.menu.setSize(mid_w, mid_h);
		this.repaintRequired = true;	
	}

}



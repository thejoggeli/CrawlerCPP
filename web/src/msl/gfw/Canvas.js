import Input        from "./Input.js"
import Camera       from "./Camera.js"
import Vec2         from "msl/util/Vec2.js"
import Fullscreen   from "msl/util/Fullscreen.js"
import Objects      from "msl/util/Objects.js"
import Log          from "msl/log/Log.js"
import Gfw          from "./Gfw.js"
import $            from "jquery"
import Subscribable from "msl/util/Subscribable.js"

class Canvas extends Subscribable {
    constructor(id, params){
        super()

        this.id = id

        // params
        this.size                   = Objects.def(params, "size", 100)
        this.sizeMode               = Objects.def(params, "sizeMode", Gfw.SIZE_MODE_FIXED_HEIGHT)
        this.width                  = Objects.def(params, "width", 0)
        this.height                 = Objects.def(params, "height", 0)
        this.clearColor             = Objects.def(params, "clearColor", null)
        this.autoClear              = Objects.def(params, "autoClear", true)
        this.autoTransformScreen    = Objects.def(params, "autoTransformScreen", true)
        this.autoTransformCamera    = Objects.def(params, "autoTransformCamera", true)
        this.outerContainer         = Objects.def(params, "container", document.body)
        
        // html
        this.$outerContainer = $(this.outerContainer)
        this.$innerContainer = $("<div class='gfw-canvas-container'>")
        this.element = document.createElement("canvas")
        this.$element = $(this.element)    
        this.$element.addClass("gfw-canvas")
        this.ctx = this.element.getContext("2d")
        Canvas.extendCtx(this.ctx)
        
        // components
        this.input = new Input(this, this.$innerContainer)
        this.camera = new Camera(this)

        // scale
        this.scale = 1

        // other
        this.clearFrame = false
        this.resized = false

    }
    setClearFrame(){
        this.clearFrame = true
    }
    install(){
        Log.print(Log.Debug, "Canvas", "install, id=" + this.id)
        this.$outerContainer.append(this.$innerContainer)
        this.$innerContainer.append(this.$element)
        this.input.install()
    }
    uninstall(){
        Log.print(Log.Debug, "Canvas", "install, id=" + this.id)
        $(this.element).remove()
        this.input.uninstall()
    }
    setFullscreen(fullscreen){
        if(fullscreen){
            this.$innerContainer.addClass("fullscreen")
            this.focus()
        } else {
            this.$innerContainer.removeClass("fullscreen")
        }
        Fullscreen.refresh()
    }
    isFullscreen(){
        return this.$innerContainer.hasClass("fullscreen")
    }
    clear(){
        this.ctx.clearRect(0, 0, this.element.width, this.element.height)
    }
    focus(){
        this.input.focus()
    }
    init(){
        this.notifySubscribers("init", this)
    }
    updateResize(){
        if(this.resized){            
            this.element.width = this.$outerContainer.innerWidth()
            this.element.height = this.$outerContainer.innerHeight()
            if(this.sizeMode == Gfw.SIZE_MODE_FIXED_HEIGHT){
                this.height = this.size
                this.width = this.element.width/this.element.height * this.size
                this.scale = this.element.height / this.height
            } else if(this.sizeMode == Gfw.SIZE_MODE_FIXED_WIDTH){
                this.width = this.size
                this.height = this.element.height/this.element.width * this.size 
                this.scale = this.element.width / this.width
            } else if(this.sizeMode == Gfw.SIZE_MODE_MIN_SIZE){
                if(this.element.width < this.element.height){
                    this.width = this.size
                    this.height = this.element.height/this.element.width * this.size 
                    this.scale = this.element.width / this.width
                } else {
                    this.height = this.size
                    this.width = this.element.width/this.element.height * this.size
                    this.scale = this.element.height / this.height
                }
            } else if(this.sizeMode == Gfw.SIZE_MODE_MAX_SIZE){
                if(this.element.width > this.element.height){
                    this.width = this.size
                    this.height = this.element.height/this.element.width * this.size 
                    this.scale = this.element.width / this.width
                } else {
                    this.height = this.size
                    this.width = this.element.width/this.element.height * this.size
                    this.scale = this.element.height / this.height
                }
            }
            this.input.resize()
            this.notifySubscribers("resize", this)
            this.resized = false
        }
    }
    updateInput(){
        this.input.update()
    }
    update(){
        this.notifySubscribers("update", this)
    }
    transformScreen(){
        this.ctx.translate(this.element.width/2, this.element.height/2)
        this.ctx.scale(this.scale, this.scale)
    }
    transformCamera(){
        this.ctx.rotate(-this.camera.angle)
        this.ctx.scale(this.camera.zoom, this.camera.zoom)
        this.ctx.translate(-this.camera.position.x, -this.camera.position.y)
    }
    renderBegin(){       
        this.ctx.save(); 
        if(this.autoClear || this.clearFrame){
            if(this.clearColor){
                this.ctx.fillStyle = this.clearColor
                this.ctx.fillRect(0,0,this.element.width, this.element.height)
            } else {
                this.ctx.clearRect(0,0,this.element.width, this.element.height)
            }
            this.clearFrame = false
        }
        if(this.autoTransformScreen){
            this.transformScreen()
        }
        if(this.autoTransformCamera){
            this.transformCamera()
        }
    }
    render(){
        this.notifySubscribers("render", this)
    }
    renderEnd(){
        this.ctx.restore()
    }
    finishFrame(){
        this.input.clearFrameKeys()
    }
    resize(){
        this.resized = true
    }
    screenToWorld(out, a){
        out.x = a.x - this.element.width/2
        out.y = a.y - this.element.height/2
        Vec2.scale(out, out, 1.0/this.scale)
        Vec2.scale(out, out, 1.0/this.camera.zoom)
        Vec2.rotate(out, out, this.camera.angle)
        Vec2.add(out, out, this.camera.position)
    }
    getOffsetLeft(){
        return this.$outerContainer[0].offsetLeft
    }
    getOffsetTop(){
        return this.$outerContainer[0].offsetTop
    }
}
Canvas.extendCtx = function(ctx){
	ctx.strokeLine = function(x1, y1, x2, y2){
		ctx.beginPath()
		ctx.moveTo(x1,y1)
		ctx.lineTo(x2,y2)
		ctx.stroke();	
	}
	ctx.strokeCircle = function(x, y, radius){
		ctx.beginPath()
		ctx.arc(x, y, radius, 0, Math.PI*2.0, false)
		ctx.stroke();	
	}
	ctx.fillCircle = function(x, y, radius){
		ctx.beginPath()
		ctx.arc(x, y, radius, 0, Math.PI*2.0, false)
		ctx.fill();	
	}
	ctx.fillArrow = function(length, thickness, head_scale){
        if(length == 0) return
		var head_thickness_half = thickness*head_scale
        var head_length = thickness*head_scale*2
        if(length-head_length < 0){
            var s = 1 + ((length-head_length)/head_length)
            length = head_length
            ctx.save()
            ctx.scale(s, s)
            ctx.beginPath()
            ctx.fillRect(0, -thickness/2, length-head_length, thickness)
            ctx.lineTo(length-head_length, -head_thickness_half)
            ctx.lineTo(length, 0)
            ctx.lineTo(length-head_length, head_thickness_half)
            ctx.fill()
            ctx.restore()
        } else {
            ctx.beginPath()
            ctx.fillRect(0, -thickness/2, length-head_length, thickness)
            ctx.lineTo(length-head_length, -head_thickness_half)
            ctx.lineTo(length, 0)
            ctx.lineTo(length-head_length, head_thickness_half)
            ctx.fill()
        }
	}
	ctx.fillQuad = function(x1, y1, x2, y2, x3, y3, x4, y4){
		ctx.beginPath()
		ctx.moveTo(x1, y1)
		ctx.lineTo(x2, y2)
		ctx.lineTo(x3, y3)
		ctx.lineTo(x4, y4)
		ctx.closePath()
		ctx.fill()
	}
	ctx.fillRoundRect = function(x, y, width, height, radius){
		ctx.pathRoundRect(x, y, width, height, radius)
		ctx.fill()
	}
	ctx.strokeRoundRect = function(x, y, width, height, radius){
		ctx.pathRoundRect(x, y, width, height, radius);	
		ctx.stroke();	
	}
	ctx.pathRoundRect = function(x, y, width, height, radius){
	/*	if (typeof stroke == 'undefined') {
			stroke = true
		}
		if (typeof radius === 'undefined') {
			radius = 5
		}
		if (typeof radius === 'number') {
			radius = {tl: radius, tr: radius, br: radius, bl: radius}
		} else {
			var defaultRadius = {tl: 0, tr: 0, br: 0, bl: 0}
			for (var side in defaultRadius) {
				radius[side] = radius[side] || defaultRadius[side]
			}
		}
		ctx.beginPath()
		ctx.moveTo(x + radius.tl, y)
		ctx.lineTo(x + width - radius.tr, y)
		ctx.quadraticCurveTo(x + width, y, x + width, y + radius.tr)
		ctx.lineTo(x + width, y + height - radius.br)
		ctx.quadraticCurveTo(x + width, y + height, x + width - radius.br, y + height)
		ctx.lineTo(x + radius.bl, y + height)
		ctx.quadraticCurveTo(x, y + height, x, y + height - radius.bl)
		ctx.lineTo(x, y + radius.tl)
		ctx.quadraticCurveTo(x, y, x + radius.tl, y)
		ctx.closePath(); */
		ctx.beginPath()
		ctx.moveTo(x + radius, y)
		ctx.lineTo(x + width - radius, y)
		ctx.quadraticCurveTo(x + width, y, x + width, y + radius)
		ctx.lineTo(x + width, y + height - radius)
		ctx.quadraticCurveTo(x + width, y + height, x + width - radius, y + height)
		ctx.lineTo(x + radius, y + height)
		ctx.quadraticCurveTo(x, y + height, x, y + height - radius)
		ctx.lineTo(x, y + radius)
		ctx.quadraticCurveTo(x, y, x + radius, y)
		ctx.closePath();	
	}
}

export default Canvas
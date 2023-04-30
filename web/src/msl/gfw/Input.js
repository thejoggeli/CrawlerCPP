import Log from "msl/log/Log.js"
import Vec2 from "msl/util/Vec2.js"
import $ from "jquery"

class Input {
    constructor(canvas, $container){
        this.canvas = canvas 
        this.$container = $container
        this.handlers = {keyDown:[], keyUp:[]}
        this.downKeys = []
        this.frameDownKeys = []
        this.frameUpKeys = []
        this.newTouches = []
        this.touches = {}
        this.numTouches = 0
        this.hasFrameKeys = false
        this.focusFrame = false
        this.blurFrame = false
        this.hasFocus = false
        this.mouse = new Input.MouseHandle(this)
        this.stickyTouch = false // whether touches expire when the finger leaves the input overlay 
        for(var i = 0; i < 256; i++){
            this.downKeys[i] = false
            this.frameDownKeys[i] = false
            this.frameUpKeys[i] = false
        }
    }
    install(){        
        this.$overlay = $("<div class='gfw-input-overlay'>")
        this.$overlay.attr("tabindex", -1)
        this.$container.append(this.$overlay)
        this.$overlay.on("keydown", (e) => {
            if(!this.downKeys[e.keyCode]){ 
                this.downKeys[e.keyCode] = true
                for(var x in this.handlers.keyDown){
                    this.handlers.keyDown[x](e.keyCode)
                }
                this.frameDownKeys[e.keyCode] = true
                this.hasFrameKeys = true
            }
        })
        this.$overlay.on("keyup", (e) => {
            this.downKeys[e.keyCode] = false
            for(var x in this.handlers.keyUp){
                this.handlers.keyUp[x](e.keyCode)
            }        
            this.frameUpKeys[e.keyCode] = true
            this.hasFrameKeys = true
        })
        this.$overlay.on("click ", (e) => {
            e.preventDefault()
            e.stopPropagation()
        })
        this.$overlay.on("mousedown ", (e) => {
            if(e.which == 1){
                this.mouse.isDown = true
                this.mouse.downFrame = true
                var touchHandle = new Input.TouchHandle(this, "mouse")
                this.newTouches.push(touchHandle)
                this.touches.mouse = touchHandle
            } else if(e.which == 2){
                this.mouse.isDown2 = true
                this.mouse.downFrame2 = true
                e.preventDefault()
                e.stopPropagation()
            } else if(e.which == 3){
                this.mouse.isDown3 = true
                this.mouse.downFrame3 = true
                e.preventDefault()
                e.stopPropagation()
            }
            this.mouse.setAbsolutePosition(e.pageX, e.pageY)
        })
        this.$overlay.on("mouseup", (e) => {
            if(e.which == 1){
                this.mouse.isDown = false
                this.mouse.upFrame = true
                if(this.touches.mouse !== undefined){
                    this.touches.mouse.expired = true
                    delete this.touches.mouse
                }
            } else if(e.which == 2){
                this.mouse.isDown2 = false
                this.mouse.upFrame2 = true
                e.preventDefault()
                e.stopPropagation()
            } else if(e.which == 3){
                this.mouse.isDown3 = false
                this.mouse.upFrame3 = true
                e.preventDefault()
                e.stopPropagation()
            }   
            this.mouse.setAbsolutePosition(e.pageX, e.pageY)
        })
        this.$overlay.on("mousemove ", (e) => {
            this.mouse.setAbsolutePosition(e.pageX, e.pageY)
            this.mouse.moveFrame = true
            if(e.target != this.$overlay && e.which != 1){
                if(this.touches.mouse !== undefined){
                    this.touches.mouse.expired = true
                    delete this.touches.mouse
                }
            }
        })
        this.$overlay.on("mouseenter", (e) => {
            this.mouse.isOnScreen = true
            this.mouse.enterScreenFrame = true
        })
        this.$overlay.on("mouseleave ", (e) => {
            if(this.mouse.isDown){
                this.mouse.upFrame = true
            }
            this.mouse.isDown = false
            this.mouse.isOnScreen = false
            this.mouse.leaveScreenFrame = true
            if(!this.stickyTouch && this.touches.mouse !== undefined){
                this.touches.mouse.expired = true
                delete this.touches.mouse
            }
        })
        this.$overlay.on("wheel", (e) => {            
            var event = e.originalEvent
            this.mouse.scrollDelta = event.wheelDeltaY/120.0
            this.mouse.setAbsolutePosition(event.pageX, event.pageY)
        })
        this.$overlay.on("blur", (e) => {
            this.releaseKeys()
            this.blurFrame = true
            this.hasFocus = false
        })
        this.$overlay.on("focus", (e) => {
            this.focusFrame = true
            this.hasFocus = true
        })
        this.$overlay.on("touchstart", (e) => {
            for(var t = 0; t < e.changedTouches.length; t++){
                var touch = e.changedTouches[t]
                var emulateMouse = this.numTouches == 0
                var touchHandle = new Input.TouchHandle(this, touch, emulateMouse)
                this.numTouches++
                this.newTouches.push(touchHandle)
                this.touches[touchHandle.id] = touchHandle
                touchHandle.setAbsolutePosition(touch.clientX, touch.clientY)
                if(touchHandle.emulateMouse){
                    this.mouse.isDown = true
                    this.mouse.isOnScreen = true
                    this.mouse.downFrame = true
                    this.mouse.setAbsolutePosition(touch.clientX, touch.clientY)
                }
            }
            e.preventDefault()
            e.stopPropagation()
        })
        this.$overlay.on("touchend touchcancel", (e) => {
            for(var t = 0; t < e.changedTouches.length; t++){
                var touch = e.changedTouches[t]
                var touchHandle = this.touches[touch.identifier]
                this.expireTouchHandle(touchHandle)
                if(touchHandle.emulateMouse){
                    this.mouse.isDown = false
                    this.mouse.upFrame = true
                    this.mouse.isOnScreen = false
                    this.mouse.setAbsolutePosition(touch.clientX, touch.clientY)
                }
            }    
            e.preventDefault()
            e.stopPropagation()
        })
        this.$overlay.on("touchmove", (e) => {
            for(var t = 0; t < e.changedTouches.length; t++){        
                var touch = e.changedTouches[t]
                var touchHandle = this.touches[touch.identifier]
                touchHandle.setAbsolutePosition(touch.clientX, touch.clientY)
                if(touchHandle.emulateMouse){
                    this.mouse.moveFrame = true
                    this.mouse.setAbsolutePosition(touch.clientX, touch.clientY)
                }
            }
            e.preventDefault()
            e.stopPropagation()
        })
    }
    expireTouchHandle(touchHandle){
        if(touchHandle.expired){
            Log.warning("Input", "expireTouchHandle() TouchHandle (id="+touchHandle+") is already expired")
            return
        }
        touchHandle.expired = true
        delete this.touches[touchHandle.id]
        this.numTouches--
    }
    uninstall(){
        this.$overlay.remove()
    }    
    update(){
    /*  for(var t in this.touches){
            var touch = this.touches[t]
            touch.updateWorldPosition()  
        } */
    }
    updateMousePosition(e){    
        this.mouse.setAbsolutePosition(e.pageX, e.pageY)
    }
    clearFrameKeys(){
        if(this.hasFrameKeys){
            for(var i = 0; i < 256; i++){
                this.frameDownKeys[i] = false
                this.frameUpKeys[i] = false
            }
            this.hasFrameKeys = false
        }
        this.mouse.moveFrame = false
        this.mouse.downFrame = false
        this.mouse.upFrame = false
        this.mouse.downFrame2 = false
        this.mouse.upFrame2 = false
        this.mouse.downFrame3 = false
        this.mouse.upFrame3 = false
        this.mouse.enterScreenFrame = false
        this.mouse.leaveScreenFrame = false        
        this.focusFrame = false
        this.blurFrame = false
        this.mouse.scrollDelta = 0
        // clear touches
        if(this.newTouches.length > 0){
            this.newTouches = []
        }
    }
    releaseKeys(){
        for(var i = 0; i < 256; i++){
            if(this.downKeys[i]){
                this.downKeys[i] = false
                this.frameUpKeys[i] = true
                this.hasFrameKeys = true
                for(var x in this.handlers.keyUp){
                    this.handlers.keyUp[x](e.keyCode)
                }      
            }
        }
        for(var id in this.touches){
            var touchHandle = this.touches[id]
            this.expireTouchHandle(touchHandle)
        }
    }
    clear(){        
        this.downKeys = []
        this.frameDownKeys = []
        this.frameUpKeys = []
        this.newTouches = []
        for(var i = 0; i < 256; i++){
            this.downKeys[i] = false
            this.frameDownKeys[i] = false
            this.frameUpKeys[i] = false
        }
        this.touches = {}
        this.hasFrameKeys = false
        this.mouse.isDown = false
        this.mouse.downFrame = false
        this.mouse.upFrame = false
    }
    isKeyDown(code){
        return (this.downKeys[code] === true)
    }
    onKeyDown(handler){
        this.handlers.keyDown.push(handler)
    }
    onKeyUp(handler){
        this.handlers.keyUp.push(handler)
    }
    keyDown(code){
        return this.frameDownKeys[code]
    }
    keyUp(code){
        return this.frameUpKeys[code]
    }
    isMouseDown(){
        return this.mouse.isDown
    }
    mouseDown(){
        return this.mouse.downFrame
    }
    mouseUp(){
        return this.mouse.upFrame
    }
    resize(){
        this.$overlay.width(this.canvas.element.width)
        this.$overlay.height(this.canvas.element.height)
    }
    focus(){
        Log.debug("Input", "focus()")
        this.$overlay.focus()
    }
}
Input.MouseHandle = class {
    constructor(input){
        this.input = input
        this.absolutePosition = Vec2.create()
        this.relativePosition = Vec2.create()
        this.worldPosition = Vec2.create()
        this.moveFrame = true
        this.isDown = false
        this.downFrame = false
        this.upFrame = false
        this.isDown2 = false
        this.downFrame2 = false
        this.upFrame2 = false
        this.isDown3 = false
        this.downFrame3 = false
        this.upFrame3 = false
        this.isOnScreen = false
        this.enterScreenFrame = false
        this.leaveScreenFrame = false
        this.scrollDelta = 0
    }
    setAbsolutePosition(x, y){
        Vec2.set(this.absolutePosition, x, y)
        this.updateRelativePosition()
        this.updateWorldPosition()
        // mouse prepends to be a touch
        var touch = this.input.touches.mouse
        if(touch){
            Vec2.copy(touch.worldPosition, this.worldPosition)
            Vec2.copy(touch.relativePosition, this.relativePosition)
            Vec2.copy(touch.absolutePosition, this.absolutePosition)
        }
    }
    updateRelativePosition(){
        this.relativePosition.x = this.absolutePosition.x - this.input.canvas.boundingClientRect.left 
        this.relativePosition.y = this.absolutePosition.y - this.input.canvas.boundingClientRect.top
    }
    updateWorldPosition(){
        this.input.canvas.screenToWorld(this.worldPosition, this.relativePosition)
    }

}
Input.TouchHandle = class {
    constructor(input, touch, emulateMouse){
        this.input = input
        this.absolutePosition = Vec2.create()
        this.relativePosition = Vec2.create()
        this.worldPosition = Vec2.create()
        this.emulateMouse = emulateMouse === undefined ? false : emulateMouse
        if(touch === "mouse"){
            this.id = "mouse_" + Input.TouchHandle.mouseIdCount++
            this.isMouse = true
            this.touch = null
        } else {
            this.id = touch.identifier
            this.isMouse = false
            this.touch = touch
        }    
        this.expired = false
        this.taken = false
    }      
    setAbsolutePosition(x, y){
        Vec2.set(this.absolutePosition, x, y)
        this.updateRelativePosition()
        this.updateWorldPosition()
    }
    updateRelativePosition(){
        this.relativePosition.x = this.absolutePosition.x - this.input.canvas.boundingClientRect.left
        this.relativePosition.y = this.absolutePosition.y - this.input.canvas.boundingClientRect.top
    }
    updateWorldPosition(){
        this.input.canvas.screenToWorld(this.worldPosition, this.relativePosition)
    }
}
Input.TouchHandle.mouseIdCount = 0

export default Input
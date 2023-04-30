import Input        from "./Input.js"
import Camera       from "./Camera.js"
import Vec2         from "msl/util/Vec2.js"
import Fullscreen   from "msl/util/Fullscreen.js"
import Objects      from "msl/util/Objects.js"
import Log          from "msl/log/Log.js"
import Gfw          from "./Gfw.js"
import $            from "jquery"
import Subscribable from "msl/util/Subscribable.js"

class CanvasThree extends Subscribable {
    constructor(id, params){
        super()

        this.id = id

        // params
        this.outerContainer = Objects.def(params, "container", document.body)
        
        // html
        this.$outerContainer = $(this.outerContainer)
        this.$innerContainer = $("<div class='gfw-canvas-container'>")
        this.element = document.createElement("canvas")
        this.$element = $(this.element)    
        this.$element.addClass("gfw-canvas")
        
        // components
        this.boundingClientRect = this.element.getBoundingClientRect()
        this.input = new Input(this, this.$innerContainer)

        this.resized = false

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
            this.input.resize()
            this.notifySubscribers("resize", this)
            this.resized = false
        }
    }
    updateInput(){
        this.boundingClientRect = this.element.getBoundingClientRect()
        this.input.update()
    }
    update(){
        this.notifySubscribers("update", this)
    }
    renderBegin(){
        
    }
    render(){
        this.notifySubscribers("render", this)
    }
    renderEnd(){

    }
    finishFrame(){
        this.input.clearFrameKeys()
    }
    resize(){
        this.resized = true
    }
    screenToWorld(out, a){

    }
    getOffsetLeft(){
        return this.boundingClientRect.left
    }
    getOffsetTop(){
        return this.boundingClientRect.top
    }
}

export default CanvasThree
import Log              from "msl/log/Log.js"
import Fullscreen       from "msl/util/Fullscreen.js"
import Arrays           from "msl/util/Arrays.js"
import FrequencyCounter from "msl/time/FrequencyCounter.js"
import Time             from "msl/time/Time.js"
import Timer            from "msl/time/Timer.js"
import Canvas           from "./Canvas.js"
import $                from "jquery"
import Subscribable     from "msl/util/Subscribable.js"
import Objects from "msl/util/Objects.js"
import CanvasThree from "./CanvasThree.js"

function Gfw(){}
Gfw.events = new Subscribable()
Gfw.installed = true
Gfw.started = false
Gfw.uninstallRequested = false
Gfw.requestAnimationFrameId = null
Gfw.fps = 0 
Gfw.mainCanvas = null
Gfw.SIZE_MODE_MIN_SIZE = 0
Gfw.SIZE_MODE_MAX_SIZE = 1
Gfw.SIZE_MODE_FIXED_WIDTH = 2
Gfw.SIZE_MODE_FIXED_HEIGHT = 3
Gfw.install = function(_params){
    Log.print(Log.Debug, "Gfw", "install")

    // params
    var params = _params === undefined ? {} : _params 

    // state variables
    Gfw.installed = true
    Gfw.started = false
    Gfw.uninstallRequested = false
    Gfw.requestAnimationFrameId = null
    Gfw.mainCanvas = null

    // fps counter
    Gfw.fps = 0 
    Gfw.fpsFrequencyCounter = new FrequencyCounter()
    Gfw.fpsTimer = new Timer()

    // canvases
    Gfw.canvases = []
    Gfw.canvases_by_id = {}

    // fullscreen
    Fullscreen.install()

    // html
    $(window).on("resize orientationchange", Gfw.resize)

    // resize just to be sure
    Gfw.resize()
}
Gfw.uninstall = function(){
    Gfw.uninstallRequested = true
}
Gfw.uninstallExecute = function(){
    Log.print(Log.Debug, "Gfw", "uninstall")
    // state variables
    Gfw.installed = false
    // canvases
    for(var i in Gfw.canvases){
        Gfw.canvases[i].uninstall()
    }
    // html
    $(window).off("resize orientationchange", Gfw.resize)
    // cancel animation frame
    window.cancelAnimationFrame(Gfw.requestAnimationFrameId)
}
Gfw.start = function(){
    if(Gfw.started){
        Log.print(Log.Error, "Gfw", "already started")
        return
    }

    // update canvases    
    for(var x in Gfw.canvases){
        Gfw.canvases[x].init()
    }

    // start time
    Time.start()

    // fps counter
    Gfw.fps = 0 
    Gfw.fpsTimer.start(Time.currentTime, 1)
    Gfw.fpsFrequencyCounter.start(Time.currentTime)

    Gfw.requestAnimationFrameId = window.requestAnimationFrame(Gfw.frame)
    Gfw.started = true
}
Gfw.getCanvas = function(id){
    if(Gfw.canvases_by_id[id] === undefined){
        return null
    }
    return Gfw.canvases_by_id[id]
}
Gfw.createCanvas = function(id, _params){
    if(!Gfw.installed){
        Log.print(Log.Error, "Gfw", "Gfw needs to be installed before a canvases can be created")
        return null
    }
    if(Gfw.getCanvas(id) != null){
        Log.print(Log.Error, "Gfw", "canvas with id=" + id + " already exists")
        return null
    }
    var params = _params === undefined ? {} : _params
    Objects.set(params, "type", "2d")
    var canvas = null
    if(params.type == "2d"){
        canvas = new Canvas(id, params)
    } else if (params.type == "three"){
        canvas = new CanvasThree(id, params)
    } else {
        Log.error("Gfw", "invalid canvas type: " + params.type)
    }
    Gfw.canvases.push(canvas)
    Gfw.canvases_by_id[id] = canvas
    canvas.install()
    if(Gfw.mainCanvas == null){
        Gfw.setMainCanvas(canvas)
    }
    // resize just to be sure
    Gfw.resize()
    return canvas
}
Gfw.destroyCanvas = function(id){
    var canvas = Gfw.getCanvas(id)
    if(canvas){
        canvas.uninstall()
        Arrays.removeObject(Gfw.canvases, canvas)
        delete Gfw.canvases_by_id[id]
    }
}
Gfw.setMainCanvas = function(canvas){
    if(Gfw.mainCanvas != canvas){
        Gfw.mainCanvas = canvas
    }
}
Gfw.resize = function(){
    for(var x in Gfw.canvases){
        Gfw.canvases[x].resize()
    }
    Gfw.events.notifySubscribers("resize", Gfw.mainCanvas)
}
Gfw.frame = function(){
    
    // update time
    Time.update()

    // fps counter
    Gfw.fpsFrequencyCounter.count()
    if(Gfw.fpsTimer.isFinished(Time.currentTime)){
        Gfw.fps = Gfw.fpsFrequencyCounter.compute(Time.currentTime)
        Gfw.fpsTimer.restart(Time.currentTime)
    }

    for(var x in Gfw.canvases){
        Gfw.canvases[x].updateResize()
        Gfw.canvases[x].updateInput()
    }

    // update canvases
    Gfw.events.notifySubscribers("beforeUpdate", Gfw.mainCanvas)
    for(var x in Gfw.canvases){
        Gfw.canvases[x].update()
    }
    Gfw.events.notifySubscribers("afterUpdate", Gfw.mainCanvas)

    // render begin
    Gfw.events.notifySubscribers("beforeRenderBegin", Gfw.mainCanvas)
    for(var x in Gfw.canvases){
        Gfw.canvases[x].renderBegin()
    }

    // render
    Gfw.events.notifySubscribers("beforeRender", Gfw.mainCanvas)
    for(var x in Gfw.canvases){
        Gfw.canvases[x].render()
    }   
    Gfw.events.notifySubscribers("afterRender", Gfw.mainCanvas)

    // render end
    for(var x in Gfw.canvases){
        Gfw.canvases[x].renderEnd()
    }
    Gfw.events.notifySubscribers("afterRenderEnd", Gfw.mainCanvas)
    
    // update canvases    
    for(var x in Gfw.canvases){
        Gfw.canvases[x].finishFrame()
    }

    // uninstall requested?
    if(Gfw.uninstallRequested){
        Gfw.uninstallExecute()
    } else {
        // next frame
        Gfw.requestAnimationFrameId = window.requestAnimationFrame(Gfw.frame)
    }
}

export default Gfw
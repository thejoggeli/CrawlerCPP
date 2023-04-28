import Log from "./Log.js"
import LogTargetBase from "./LogTargetBase.js"
import DateTime from "msl/time/DateTime.js"

class LogTargetConsole extends LogTargetBase {
    constructor(){
        super()
        this.use_colors = true
        for(var e in Log.Levels.entries){
            Log.Levels.entries[e].color_console = "" 
        }
        Log.Info.color_console = "color: green"
        Log.Error.color_console = "color: red"
        Log.Warning.color_console = "color: orange"
        Log.Unknown.color_console = ""
        Log.Debug.color_console = "color: magenta"
    }
    print(level, source, ...message){
        if(level == Log.Error){
            console.error(...message)
        } else if(level == Log.Warning){
            console.warn(...message)
        }
        var time = DateTime.getTimeString();
        var prefix = "[" + level.short + "|" + time + "|" + source + "]"
        if(this.use_colors && level.color_console != ""){
            console.log("%c" + prefix, level.color_console, ...message)
        } else {
            console.log(prefix, ...message)
        }
    }
}

export default LogTargetConsole
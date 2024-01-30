import Time from "msl/time/Time";
import Subscribable from "msl/util/Subscribable";
import PacketMessage from "./PacketMessage";
import Main from "./Main";

export default class Console {

    static LineTypeError = 0
    static LineTypeCommand = 1
    static LineTypeDefault = 2

    static nextLineId = 0
    static events = new Subscribable()
    static linesById = {}
    static lines = []
    static maxLines = 100
    static submittedLines = []

    static init(){
        this.addLine("type 'help' for a list of possible commands")
    }

    static submitText(text){
        text = text.trim()
        if(text == ""){
            return
        }
        var ret = this.parseLine(text)
        if(ret.precall){
            ret.precall() 
        }
        var line = this.addLine(text, ret.parsed ? Console.LineTypeCommand : Console.LineTypeDefault)
        if(ret.error){
            this.addLine(ret.error, Console.LineTypeError)
        }
        if(ret.postcall){
            ret.postcall() 
        }
        for (var i = this.submittedLines.length - 1; i >= 0; i--) {
            if(this.submittedLines[i].text == line.text){
                this.submittedLines.splice(i, 1);   
            }
        }
        this.submittedLines.push(line)   
    }
    
    static addLine(text, type){
        var id = this.nextLineId
        var line = {
            timestamp: Time.getSystemTimeMillis(),
            id: id,
            text: text,
            type: type,
        }
        this.linesById[id] = line
        this.lines.push(line)
        this.events.notifySubscribers("addLine", line)
        while(this.lines.length > this.maxLines){
            this.events.notifySubscribers("removeLine", this.lines[0])
            delete this.lines[0]
            this.lines.shift()
        }
        this.nextLineId += 1
        return line
    }

    static parseLine(text){
        text = text.toLowerCase()
        var split = text.split(" ")
        for(var i in split){
            split[i] = split[i].trim()
        }
        var parsed = false
        var cmd = split[0]
        var msg = null
        var err = null
        var precall = null
        var postcall = null
        if(cmd == "torque"){
            msg = new PacketMessage("setTorque")
            if(split[1] == "on" || split[1] == "off"){
                msg.addBool("torque", split[1] == "on" ? true : false)
            } else {
                err = "invalid argument for torque command. try 'on' or 'off'"
            }
        } else if(cmd == "reboot"){
            msg = new PacketMessage("reboot")
        } else if(cmd == "exit"){
            msg = new PacketMessage("exit")
        } else if(cmd == "brain"){
            if(split.length == 1){
                msg = new PacketMessage("printBrain")
            } else {
                msg = new PacketMessage("setBrain")
                msg.addString("name", split[1])
            }
        } else if(cmd == "body"){
            msg = new PacketMessage("setBodyPosition")
            err = "invalid argument for body command"
            // TODO 
            // parse command
        } else if(cmd == "foot"){
            msg = new PacketMessage("setJointAngle")
            err = "invalid argument for foot command"
            // TODO 
            // parse command
        } else if(cmd == "joint"){
            msg = new PacketMessage("setFootPosition")
            err = "invalid argument for joint command"
        } else if(cmd == "print"){
            var what = split[1]
            var parsed = true
            if(what == "status"){
                msg = new PacketMessage("printStatus")
            } else if (what == "angles"){
                msg = new PacketMessage("printAngles")
            } else if (what == "positions"){
                msg = new PacketMessage("printPositions")
            } else {
                err = "invalid argument for print command"
            }
        } else if(cmd == "help"){
            parsed = true
            postcall = () => {
                this.printHelp()
            }
        } else if(cmd == "brains"){
            parsed = true
            postcall = () => {
                this.printBrains()
            }
        } else if(cmd == "clear"){
            parsed = true
            precall = () => {
                this.clear()
            }
            postcall = () => {
                this.addLine("console was cleared. type 'help' for a list of possible commands")
            }
        } else {
            err = "unknown command " + cmd
        }
        if(msg && !err){
            Main.addPacketMessage(msg)
        }
        if(msg != null){
            parsed = true
        }
        return {
            parsed: parsed,
            error: err,
            precall: precall,
            postcall: postcall,
        }
    }

    static clear(){
        this.events.notifySubscribers("clear")
        this.lines = []
        this.linesById = {}
    }

    static printHelp(){
        this.addLine("exit - shut down server")
        // this.addLine("reboot - reboot servo motors")
        this.addLine("torque <on/off> - enable or disable servo motors")
        this.addLine("brain <brain> - set the brain. type 'brains' for a list of brains")
        // this.addLine("body - todo")
        // this.addLine("foot - todo")
        // this.addLine("joint - todo")
        this.addLine("print <status/angles/positions>")
        this.addLine("clear - clears this console")
    }

    static printBrains(){
        this.addLine("calib")
        this.addLine("surf-1")
        this.addLine("surf-2")
        this.addLine("walk")
        this.addLine("dance")
    }
    
}
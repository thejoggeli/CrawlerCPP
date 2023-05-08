import Time from "msl/time/Time";
import Subscribable from "msl/util/Subscribable";
import PacketMessage from "./PacketMessage";
import Main from "./Main";

export default class Console {

    static nextLineId = 0
    static events = new Subscribable()
    static linesById = {}
    static lines = []
    static maxLines = 100

    static init(){
        
    }

    static submitText(text){
        text = text.trim()
        if(text == ""){
            return
        }
        var parsed = this.parseLine(text)
        this.addLine(text, parsed)
    }
    
    static addLine(text, parsed){
        var id = this.nextLineId
        var line = {
            timestamp: Time.getSystemTimeMillis(),
            id: id,
            text: text,
            parsed: parsed,
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
    }

    static parseLine(text){
        text = text.toLowerCase()
        var split = text.split(" ")
        for(var i in split){
            split[i] = split[i].trim()
        }
        var cmd = split[0]
        var msg = null
        if(cmd == "torque"){
            if(split[1] == "on" || split[1] == "off"){
                msg = new PacketMessage("setTorque")
                msg.addBool(split[1] == "on" ? true : false)
            }
        } else if(cmd == "reboot"){
            msg = new PacketMessage("reboot")
        } else if(cmd == "exit"){
            msg = new PacketMessage("exit")
        } else if(cmd == "brain"){
            msg = new PacketMessage("setBrain")
            msg.addString(split[1])
        } else if(cmd == "body"){
            msg = new PacketMessage("setBodyPosition")
            // TODO 
            // parse command
        } else if(cmd == "foot"){
            msg = new PacketMessage("setJointAngle")
            // TODO 
            // parse command
        } else if(cmd == "joint"){
            msg = new PacketMessage("setFootPosition")
        } else if(cmd == "print"){
            var what = split[1]
            if(what == "status"){
                msg = new PacketMessage("printStatus")
            } else if (what == "angles"){
                msg = new PacketMessage("printAngles")
            } else if (what == "positions"){
                msg = new PacketMessage("printPositions")
            }
        }
        if(msg){
            Main.addPacketMessage(msg)
            return true
        }
        return false
    }
    
}
import Log from "msl/log/Log.js"
import Packet from "./Packet.js"

export default class PacketRecycler {
    constructor(){
        this.groups = {}
        this.groupKeys = []
    }
    createGroup(key){
        if(this.groups[key] !== undefined){
            Log.debug("PacketRecycler", "group does already exist, key="+key)
            return
        }
        Log.debug("PacketRecycler", "creating group, key="+key)
        this.groups[key] = new PacketRecyclerGroup(key)
        this.groupKeys = Object.keys(this.groups)
    }
    hasGroup(key){
        return this.groups[key] !== undefined
    }
    popPacket(key){
        if(this.groups[key] === undefined){
            this.createGroup(key)
            return new Packet()
        }
        var group = this.groups[key]
        if(group.packets.length == 0){
            return new Packet()
        } else {
            return group.packets.pop()
        }
    }
    pushPacket(key, packet){
        if(this.groups[key] === undefined){
            this.createGroup(key)
        }        
        var group = this.groups[key]
        group.packets.push(packet)
    }
}

class PacketRecyclerGroup {
    constructor(key){
        this.key = key
        this.packets = []
    }
}
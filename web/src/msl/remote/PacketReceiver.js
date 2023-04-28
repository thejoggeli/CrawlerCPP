import Log from "msl/log/Log.js"
import Packet from "msl/remote/Packet.js"
import Subscribable from "msl/util/Subscribable.js"

class PacketReceiver extends Subscribable {
    constructor(){
        super()
        this.packets = []
        this.logDebug = false
    }
    addPacket(packet){
        // Log.debug("PacketReceiver", "Received " + packet.type.name)
        if(this.logDebug){
            Log.debug("PacketReceiver", packet.type.name)
        }
        this.packets.push(packet)
        // console.log(packet)
    }
    getPackets(){
        return this.packets
    }
    clearPackets(){
        if(this.packets.length > 0){
            this.packets = []
        }
    }
    processPackets(){
        if(this.packets.length > 0){
            // first clear packets queue and keep a reference to the old current
            // this way subscribers can add new packets inside their event listeners 
            var packets = this.packets
            this.clearPackets()
            // notify subscribers
            for(var p in packets){
                var packet = packets[p]
                this.notifySubscribers(packet.type.name, packet)
            }
        }
    }
}

export default PacketReceiver

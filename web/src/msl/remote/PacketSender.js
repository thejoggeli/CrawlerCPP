import Log from "msl/log/Log.js"

class PacketSender {
    constructor(){
        this.packets = []
        this.logDebug = false
    }
    addPacket(packet){
        this.packets.push(packet)
        if(this.logDebug){
            console.log(packet.type.name, packet.data)
        }
    }
    getPackets(){
        return this.packets
    }
    clearPackets(){
        if(this.packets.length > 0){
            this.packets = []
        }
    }
    processAll(){
        Log.error("PacketSender", "processAll() not implemented")
    }
}


export default PacketSender
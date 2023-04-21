import Log from "msl/log/Log.js"

class PacketSender {
    constructor(){
        this.packets = []
    }
    addPacket(packet){
        this.packets.push(packet)
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
import Packet from "msl/remote/Packet"

export default class PacketMessage  {

    constructor(p){
        if(p instanceof Packet){
            this.packet = p
            this.message = p.data.message
            this.params = p.data.params
        } else if(typeof p == "string") {
            this.packet = null
            this.message = p
            this.params = {}
        } else {
            this.packet = null
            this.message = ""
            this.params = {}
        }
    }

    setMessage(message){
        this.message = message
    }

    getFloat(key){
        return parseFloat(this.params[key])
    }

    getInt(key){
        return parseInt(this.params[key])
    }

    getBool(key){
        return this.params[key] == "0" ? false : true
    }

    getString(key){
        return this.params[key]
    }

    addFloat(key, val){
        this.params[key] = val.toExponential()
    }

    addInt(key, val){
        this.params[key] = val+""
    }

    addBool(key, val){
        this.params[key] = val ? '1' : '0'
    }

    addString(key, val){
        this.params[key] = val
    }

}
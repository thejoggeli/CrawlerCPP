import Subscribable from "msl/util/Subscribable"
import Log from "msl/log/Log"

export default class Connection extends Subscribable { 
    constructor(_params){
        super()
        var params = _params ? _params : {}
        this.defaultProtocol = "ws"
        this.defaultHost = window.location.hostname ? window.location.hostname : "localhost"
        this.defaultPort = 8080
        this.defaultPath = "ws"
        this.protocol = params.protocol ? params.protocol : this.defaultProtocol
        this.host     = params.host     ? params.host     : this.defaultHost
        this.port     = params.port     ? params.port     : this.defaultPort
        this.path     = params.path     ? params.path     : this.defaultPath
        this.url = this.protocol + "://" + this.host + ":" + this.port + "/" + this.path
        this.connected = false
        this.websocket = null
    }
    connect(){
        this.websocket = new WebSocket(this.url)
        this.websocket.binaryType = "arraybuffer";
        this.websocket.addEventListener("open", (event) => {
            this.connected = true  
            // Log.info("Connection", "connected=" + this.connected)
            this.notifySubscribers("Open", event)
        })
        this.websocket.addEventListener("close", (event) => {
            if(this.connected){
                this.connected = false
                // Log.info("Connection", "connected=" + this.connected)
                this.notifySubscribers("Close", event)
            } else {
                this.connected = false
                // Log.info("Connection", "connected=" + this.connected)
                this.notifySubscribers("OpenFailed", event)
            }
        })
        this.websocket.addEventListener("error", (event) => {
            // Log.error("Connection", event)
            this.notifySubscribers("Error", event)
        })
        this.websocket.addEventListener("message", (event) => {
            // Log.info("Connection", "message", event)
            this.notifySubscribers("Message", event)
        })
    }
    send(data){
        this.websocket.send(data)
    }
}

